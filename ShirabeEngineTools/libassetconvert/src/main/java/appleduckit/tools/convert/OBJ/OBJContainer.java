package  appleduckit.tools.convert.OBJ;

public class OBJContainer {

	String _name = null;
	boolean _hasNormals = false;
	boolean _hasTexCoords = false;
	int _vertexPositionComponentCount = 0;
	int _vertexNormalComponentCount = 0;
	int _vertexTexCoordComponentCount = 0;
	Float[] _interleavedBuffer = null;
	Short[] _indexBuffer = null;

	public void setName(String name) {
		_name = name;
	}

	public String getName() {
		return _name;
	}

	public void setVertexPositionComponentCount(int count) {
		_vertexPositionComponentCount = count;
	}

	public int getVertexPositionComponentCount() {
		return _vertexPositionComponentCount;
	}

	public void setVertexNormalComponentCount(int count) {
		_vertexNormalComponentCount = count;
	}

	public int getVertexNormalComponentCount() {
		return _vertexNormalComponentCount;
	}

	public void setVertexTexCoordComponentCount(int count) {
		_vertexTexCoordComponentCount = count;
	}

	public int getVertexTexCoordComponentCount() {
		return _vertexTexCoordComponentCount;
	}

	public void setInterleavedBuffer(Float[] buffer) {
		_interleavedBuffer = buffer;
	}

	public Float[] getInterleavedBuffer() {
		return _interleavedBuffer;
	}

	public void setIndexBuffer(Short[] indexBuffer) {
		_indexBuffer = indexBuffer;
	}

	public Short[] getIndexBuffer() {
		return _indexBuffer;
	}

	public void setHasNormals(boolean b) {
		_hasNormals = b;

	}

	public void setHasTexCoords(boolean b) {
		_hasTexCoords = b;

	}
}
