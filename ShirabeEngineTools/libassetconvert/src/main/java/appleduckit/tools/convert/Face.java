package  appleduckit.tools.convert;

import java.security.InvalidParameterException;
import java.util.LinkedList;
import java.util.List;

public class Face {

	List<FaceVertex> _vertices = null;

	public Face(FaceVertex fv0, FaceVertex fv1, FaceVertex fv2) {
		_vertices = new LinkedList<FaceVertex>();
		_vertices.add(fv0);
		_vertices.add(fv1);
		_vertices.add(fv2);
	}

	public List<FaceVertex> getVertices() {
		return _vertices;
	}

	public static Face parse(String strFace, boolean flipWinding) {
		String[] strFaces = strFace.split(" ");
		if (strFaces.length != 3)
			throw new InvalidParameterException("Invalid face list format");

		FaceVertex fv0 = FaceVertex.parse(strFaces[0]);
		FaceVertex fv1 = FaceVertex.parse(strFaces[1]);
		FaceVertex fv2 = FaceVertex.parse(strFaces[2]);

		Face f = null;
		if (flipWinding)
			f = new Face(fv2, fv1, fv0);
		else
			f = new Face(fv0, fv1, fv2);

		return f;
	}
}
