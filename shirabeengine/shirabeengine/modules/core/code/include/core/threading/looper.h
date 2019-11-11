#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <stdexcept>
#include <mutex>
#include <queue>

class CLooper
{

public:
    using Runnable = std::function<void()>;

    class CHandler
    {
        friend class CLooper; // Allow the looper to access the private constructor.

    public:
        bool post(CLooper::Runnable &&aRunnable)
        {
            return mAssignedLooper.post(std::move(aRunnable));
        }

    private: // construction, since we want the looper to expose it's handler exclusively!
        CHandler(CLooper &aLooper)
                : mAssignedLooper(aLooper)
        {}

    private:
        CLooper &mAssignedLooper;
    };

public:
    CLooper()
            : mRunning(false)
              , mAbortRequested(false)
              , mRunnablesMutex({})
              , mRunnables()
              , mHandler(std::shared_ptr<CHandler>(new CHandler(*this)))
    { }
    // Copy denied, Move to be implemented

    ~CLooper()
    {
        abortAndJoin();
    }

    [[nodiscard]]
    bool initialize()
    {
        if(running())
        {
            return false;
        }

        // We will put some initialization here

        return true;
    }

    [[nodiscard]]
    bool deinitialize()
    {
        if(running())
        {
            // Stop first
            return false;
        }

        // We will put some deinitialization here
        mAbortRequested.store(false);

        return true;

    }

    [[nodiscard]]
    bool running() const
    {
        return mRunning.load();
    }

    [[nodiscard]]
    bool run()
    {
        try
        {
            mThread = std::thread(&CLooper::runFunc, this);
        }
        catch(...)
        {
            return false;
        }

        return true;
    }

    void stop()
    {
        abortAndJoin();
    }

    std::shared_ptr<CHandler> getHandler()
    {
        return mHandler;
    }

private:
    void runFunc()
    {
        mRunning.store(true);

        while(false == mAbortRequested.load())
        {
            try
            {
                Runnable r = next();
                if(nullptr != r)
                {
                    r();
                }
            }
            catch(std::runtime_error& e)
            {
                // Some more specific
            }
            catch(...)
            {
                // Make sure that nothing leaves the thread for now...
            }
        }

        mRunning.store(false);
    }

    void abortAndJoin()
    {
        mAbortRequested.store(true);
        if(mThread.joinable())
        {
            mThread.join();
        }
    }

    // Runnables
    Runnable next()
    {
        std::lock_guard<std::recursive_mutex> guard(mRunnablesMutex);

        if(mRunnables.empty())
        {
            return nullptr;
        }
        Runnable runnable = mRunnables.front();
        mRunnables.pop();

        return runnable;
    }

    bool post(Runnable &&aRunnable)
    {
        try
        {
            std::lock_guard<std::recursive_mutex>
                    guard(mRunnablesMutex);

            mRunnables.push(std::move(aRunnable));
        }
        catch(...) {
            return false;
        }

        return true;
    }

private:
    std::thread      mThread;
    std::atomic_bool mRunning;
    std::atomic_bool mAbortRequested;

    std::queue<Runnable>  mRunnables;
    std::recursive_mutex  mRunnablesMutex;

    std::shared_ptr<CHandler> mHandler;
};


#include <iostream>

int main()
{
    std::unique_ptr<CLooper> looper = std::make_unique<CLooper>();

    std::cout << "Starting looper" << std::endl;
    // To start and run
    SHIRABE_EXPLICIT_DISCARD(looper->initialize());
    SHIRABE_EXPLICIT_DISCARD(looper->run());

    std::shared_ptr<CLooper::CHandler> handler = looper->getHandler();

    std::cout << "Adding tasks" << std::endl;

    for(uint32_t k=0; k<500; ++k)
    {
        auto const task = [k]()
        {
            std::cout << "Invocation " << k
                      << ": Hello, I have been executed asynchronously on the looper for " << (k + 1)
                      << " times." << std::endl;
        };

        handler->post(std::move(task));
    }

    std::cout << "Waiting 5 seconds for completion" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Stopping looper" << std::endl;
    // To stop it and clean it up
    looper->stop();
    SHIRABE_EXPLICIT_DISCARD(looper->deinitialize());
    looper = nullptr;
}
