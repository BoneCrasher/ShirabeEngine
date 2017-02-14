package  appleduckit.tools.convert.OBJ;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

import appleduckit.tools.convert.Face;
import appleduckit.tools.convert.FaceVertex;

public class OBJParser {

	public static String readToEnd(StringReader rdr) throws IOException {
		char c = 0;
		int i = -1;
		String s = "";
		while ((i = rdr.read()) != -1)
			s += (char) i;

		return s;
	}

	public static String readTerm(StringReader rdr) throws IOException {
		String term = "";
		int i = 0;
		char s;
		while ((i = rdr.read()) != -1 && ((char) i) != ' ')
			term += ((char) i);

		return term.equalsIgnoreCase("") ? null : term;
	}

	public static Float readFloat(StringReader rdr) throws Exception {
		String strFloat = readTerm(rdr);
		if (strFloat == null || strFloat.length() == 0) {
			return null;
		}

		return Float.parseFloat(strFloat);
	}

	public static Face readFace(StringReader rdr) throws IOException {
		String v0 = readTerm(rdr);
		String v1 = readTerm(rdr);
		String v2 = readTerm(rdr);

		FaceVertex fv0 = FaceVertex.parse(v0);
		FaceVertex fv1 = FaceVertex.parse(v1);
		FaceVertex fv2 = FaceVertex.parse(v2);

		return new Face(fv0, fv1, fv2);
	}

	public static void expand(List<Face> faces, int vertexCount,
			int texCoordCount, int normalCount,
			Map<Integer, Integer> newVertexMap,
			Map<Integer, Integer> newNormalMap,
			Map<Integer, Integer> newTexCoordMap) {

		List<FaceVertex>[][][] faceMap = (List<FaceVertex>[][][]) new List[vertexCount][texCoordCount][normalCount];

		for (Face f : faces) {
			// Loop through the vertices in order!
			for (FaceVertex fv : f.getVertices()) {
				int v = fv.getVertexIndex() - 1;
				int t = fv.getTexCoordIndex() - 1;
				int n = fv.getNormalIndex() - 1;

				if (faceMap[v][t][n] == null)
					faceMap[v][t][n] = new LinkedList<FaceVertex>();

				faceMap[v][t][n].add(fv);
			}
		}

		int vertexIndex = 0;
		int normalIndex = 0;
		int texCoordIndex = 0;

		for (int v = 0; v < vertexCount; ++v)
			for (int t = 0; t < texCoordCount; ++t)
				for (int n = 0; n < normalCount; ++n) {
					if (faceMap[v][t][n] == null)
						// No vertices for this permutation
						continue;

					// Expand the new face vertex assignment...
					vertexIndex = newVertexMap.size();
					normalIndex = newNormalMap.size();
					texCoordIndex = newTexCoordMap.size();
					newVertexMap.put(vertexIndex, v);
					newTexCoordMap.put(texCoordIndex, t);
					newNormalMap.put(normalIndex, n);

					// And assign to all affected face vertices
					// Since the face vertices itself arent removed from the
					// actual faces,
					// thei'll still be in the correct order.
					for (FaceVertex fv : faceMap[v][t][n]) {

						fv.setVertexIndex(vertexIndex);
						fv.setNormalIndex(normalIndex);
						fv.setTexCoordIndex(texCoordIndex);
					}
				}
	}

	public static OBJContainer parse(String filename, int conversionFlags)
			throws Exception {
		String name = null;

		int vertexComponentCount = -1;
		int normalComponentCount = -1;

		List<Float> vertices = new LinkedList<Float>();
		List<Float> texcoords = new LinkedList<Float>();
		List<Float> normals = new LinkedList<Float>();
		List<Short> indices = new LinkedList<Short>();

		int faceListIndex = 0;
		List<Face> faces = new LinkedList<Face>();

		File file = new File(filename);
		Scanner scan = new Scanner(file);

		Float value = null;
		int comp = 0;
		String mode = null;
		Face f = null;

		boolean flipUVyCoord = ((conversionFlags & OBJConversionFlags.UVFlipY
				.flag()) > 0);
		boolean flipWinding = ((conversionFlags & OBJConversionFlags.FlipTriangleWindingOrder
				.flag()) > 0);

		System.out.println("Extracting contents of OBJ-file.");

		while (scan.hasNextLine()) {
			String line = scan.nextLine();
			StringReader rdr = new StringReader(line);

			String cmd = readTerm(rdr);

			if (cmd.equalsIgnoreCase("o")) {

				name = readTerm(rdr);
			} else if (cmd.equalsIgnoreCase("v")) {
				comp = 0;
				while ((value = readFloat(rdr)) != null) {
					vertices.add((float) value);
					++comp;
				}

				vertexComponentCount = comp;
			} else if (cmd.equalsIgnoreCase("vt")) {
				comp = 0;
				while ((value = readFloat(rdr)) != null) {
					// If --uvFlipY was passed, do so for the second, thus y
					// component.
					if (comp == 1 && flipUVyCoord)
						value = 1f - (float) value;

					texcoords.add((float) value);
					++comp;
				}

			} else if (cmd.equalsIgnoreCase("vn")) {
				comp = 0;
				while ((value = readFloat(rdr)) != null) {
					normals.add((float) value);
					++comp;
				}

				normalComponentCount = comp;

			} else if (cmd.equalsIgnoreCase("s")) {
				mode = readTerm(rdr);
				if (mode == "off")
					faceListIndex = 0;
				else
					faceListIndex = 1;

			} else if (cmd.equalsIgnoreCase("f")) {
				f = Face.parse(readToEnd(rdr), flipWinding);

				faces.add(f);
			}
			rdr.close();
		}

		int vertexCount = vertices.size() / vertexComponentCount;
		int normalCount = normals.size() / normalComponentCount;
		int texCoordCount = texcoords.size() / 2; // u + v
		System.out.println("Original counts:\nVertices:" + vertexCount
				+ " Normals:" + normalCount + " TexCoords:" + texCoordCount);
		System.out.println("Expanding");

		// Expand the optimized data structures within the OBJ so that we have
		// per vertex normals and texCoords.
		Map<Integer, Integer> newVertexMap = new LinkedHashMap<Integer, Integer>();
		Map<Integer, Integer> newNormalMap = new LinkedHashMap<Integer, Integer>();
		Map<Integer, Integer> newTexCoordMap = new LinkedHashMap<Integer, Integer>();

		// When attempting to get per vertex normals and texCoords there will be
		// a possibly large amount of duplication.
		// This is due to the lack of smoothing groups.
		// In the best case only vertices that share n smoothing groups (n>1)
		// will be duplicated n-1 times.
		// However, if no smoothing is used at all, each face, i.e. each
		// triangle can be interpreted as a distinct
		// Smoothing group and consequently each vertex is duplicated n-1 times
		// if it is part of n triangles!
		// In the worst case that means that there will be numQuads * 4 vertices
		// in the buffer.
		expand(faces, vertexCount, texCoordCount, normalCount, newVertexMap,
				newNormalMap, newTexCoordMap);

		// Create the interleaved vertex buffer in the format PPPNNNTT
		vertexCount = newVertexMap.size();
		normalCount = newNormalMap.size();
		texCoordCount = newTexCoordMap.size();
		int stride = vertexComponentCount + normalComponentCount + 2;
		int totalCount = vertexCount * vertexComponentCount + normalCount
				* normalComponentCount + texCoordCount * 2;
		Float[] interleavedVertexBuffer = new Float[totalCount];

		System.out.println("New counts:\nVertices:" + vertexCount + " Normals:"
				+ normalCount + " TexCoords:" + texCoordCount);

		for (int i = 0; i < vertexCount; ++i) {
			int sourceIndex = newVertexMap.get(i) * vertexComponentCount;
			int targetIndex = i * stride;
			for (int v = 0; v < vertexComponentCount; ++v)
				interleavedVertexBuffer[targetIndex + v] = vertices
						.get(sourceIndex + v);

			// System.arraycopy(vertexArray, sourceIndex,
			// interleavedVertexBuffer,
			// i * stride, vertexComponentCount);

			sourceIndex = newNormalMap.get(i) * normalComponentCount;
			targetIndex += vertexComponentCount;
			for (int v = 0; v < normalComponentCount; ++v)
				interleavedVertexBuffer[targetIndex + v] = normals
						.get(sourceIndex + v);

			// System.arraycopy(normalArray, sourceIndex,
			// interleavedVertexBuffer,
			// i * stride + vertexComponentCount, normalComponentCount);

			sourceIndex = newTexCoordMap.get(i) * 2;
			targetIndex += normalComponentCount;
			for (int v = 0; v < 2; ++v) {
				interleavedVertexBuffer[targetIndex + v] = texcoords
						.get(sourceIndex + v);
			}

			// System.arraycopy(texCoordArray, sourceIndex,
			// interleavedVertexBuffer, i * stride + vertexComponentCount
			// + normalComponentCount, 2);
		}

//		System.out.println("\nInterleaved Buffer:\n");
//		for (Float val : interleavedVertexBuffer)
//			System.out.println(val + ", ");
//		System.out.println();

		// Generate index buffer

		for (Face face : faces)
			for (FaceVertex fv : face.getVertices())
				indices.add(new Short((short) (int) fv.getVertexIndex()));

		Short[] indexBuffer = new Short[indices.size()];
		indices.toArray(indexBuffer);

		OBJContainer container = new OBJContainer();
		container.setName(name);
		container.setHasNormals(true);
		container.setHasTexCoords(true);
		container.setVertexPositionComponentCount(vertexComponentCount);
		container.setVertexNormalComponentCount(normalComponentCount);
		container.setVertexTexCoordComponentCount(2);
		container.setInterleavedBuffer(interleavedVertexBuffer);
		container.setIndexBuffer(indexBuffer);

		return container;
	}
}
