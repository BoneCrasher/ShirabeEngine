package  appleduckit.tools.convert;

import java.security.InvalidParameterException;

public class FaceVertex {
	Integer _vertexIndex = null;
	Integer _normalIndex = null;
	Integer _texCoordIndex = null;

	public FaceVertex(Integer vi, Integer ti, Integer ni) {
		_vertexIndex = vi;
		_normalIndex = ni;
		_texCoordIndex = ti;
	}

	public Integer getVertexIndex() {
		return _vertexIndex;
	}

	public Integer getNormalIndex() {
		return _normalIndex;
	}

	public Integer getTexCoordIndex() {
		return _texCoordIndex;
	}

	public void setVertexIndex(Integer vertexIndex) {
		_vertexIndex = vertexIndex;

	}

	public void setNormalIndex(Integer normalIndex) {
		_normalIndex = normalIndex;

	}

	public void setTexCoordIndex(Integer texCoordIndex) {
		_texCoordIndex = texCoordIndex;
	}

	public static FaceVertex parse(String strFV) {
		String[] indices = strFV.split("/");
		if (indices.length != 3)
			throw new InvalidParameterException("Invalid input format.");

		Integer vi = Integer.parseInt(indices[0]);
		Integer ti = Integer.parseInt(indices[1]);
		Integer ni = Integer.parseInt(indices[2]);

		return new FaceVertex(vi, ti, ni);
	}
}
