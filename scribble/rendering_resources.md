Passes define two phases:

1. Setup

During setup, all use of resources is declared, yielding 0 or more framegraph resource derivates describing the resource types and metrics 
used during the creation process of graphics API entities.

This in information will be made available in specific information blocks within asset.desc files (JSON).
Each processed resource will be associated at least one .desc-file.

As such, all information necessary to create the resource containers is available.

2. Execute

During execution all resources are available at least in staging buffers.
The render backend will then create API objects, bind buffers, textures and other objects, and will dispatch draw calls.

This will yield at least two recorded command buffers:

    a. Transfer-CommandBuffer
        
The transfer command buffer will record all kinds of operations, which are static transitions, like uniform buffer updates, readonly texture transfers, vertex buffer streams, and so on.
This buffer will be submitted and executed first, synchronized by a semaphore.
    
    b. Render-CommandBuffer
    
The render command buffer will contain all buffer and texture binding to the respective indices as well as drawing calls.
These commands perform the actual render work and will be executed after all static resource transfers and can contain dynamic resource transfers, like procedurally generated/altered textures, compute-shader computed values and texture inputs etc.


3. Role of the resource manager

One crucial aspect of the resource manager is handling resource creation requests.
