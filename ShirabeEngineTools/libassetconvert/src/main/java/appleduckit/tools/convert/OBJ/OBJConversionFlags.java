package appleduckit.tools.convert.OBJ;

public enum OBJConversionFlags {
	None(0),
	UVFlipY(1),
	FlipTriangleWindingOrder(2),
	WritePlainText(4);

	private int _value;

	private OBJConversionFlags(int value) {
		_value = value;
	}

	public int flag() {
		return _value;
	}
}
