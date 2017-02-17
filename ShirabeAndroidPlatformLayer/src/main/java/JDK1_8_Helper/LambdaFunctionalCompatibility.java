package JDK1_8_Helper;

/**
 * Created by Shirabe on 17/02/2017.
 */

public class LambdaFunctionalCompatibility {
	public interface SingleTypePredicate<T> {
		boolean test(T lhs, T rhs);
	}

	public interface MultiTypePredicate<T, U> {
		boolean test(T lhs, U rhs);
	}
}
