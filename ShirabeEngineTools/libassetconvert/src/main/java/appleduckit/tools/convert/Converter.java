package appleduckit.tools.convert;

import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Locale;
import java.util.Map.Entry;
import java.util.concurrent.SynchronousQueue;
import java.util.logging.ConsoleHandler;

import appleduckit.tools.convert.OBJ.OBJContainer;
import appleduckit.tools.convert.OBJ.OBJConversionFlags;
import appleduckit.tools.convert.OBJ.OBJParser;

public class Converter {
	public static final int Flag_SourceFound = 1;
	public static final int Flag_TargetFound = 2;

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		// args = new String[] { "--uvFlipY", "--flipWindingOrder",
		// "'Test.obj'",
		// ">'Test.semodel'" };
		String             targetFilename = null;
		LinkedList<String> sourceFiles    = new LinkedList<String>();

		int validationFlags = 0;
		int conversionFlags = OBJConversionFlags.None.flag();

		for (int i = 0; i < args.length; ++i) {
			if (args[i].startsWith("--")) { // Read option
				String command = args[i].substring(2, args[i].length())
										 .toLowerCase();
				if (command.equalsIgnoreCase("uvFlipY")) {
					conversionFlags |= OBJConversionFlags.UVFlipY.flag();
				} else if (command.equalsIgnoreCase("flipWindingOrder")) {
					conversionFlags |= OBJConversionFlags.FlipTriangleWindingOrder.flag();
				} else if (command.equalsIgnoreCase("writePlainText")) {
					conversionFlags |= OBJConversionFlags.WritePlainText.flag();
				} else {

				}
			} else if (args[i].startsWith("target:")) { // Read target file
				validationFlags |= Flag_TargetFound;
				// TODO: Make method I
				targetFilename = args[i];
				targetFilename = targetFilename.substring(("target:").length(), targetFilename.length() - 1);
				if (targetFilename.startsWith("'") || targetFilename.startsWith("\""))
					targetFilename = targetFilename.substring(1,
															  targetFilename.length() - 2);
				if(!targetFilename.endsWith(".semodel"))
					targetFilename = targetFilename + ".semodel";

				System.out.println("Output-Filename: " + targetFilename);

			} else { // Read source file
				// TODO: Make method II
				String fn = args[i];
				if (fn.startsWith("'") || fn.startsWith("\""))
					fn = fn.substring(1, args[i].length() - 2);

				File f = new File(fn);
				if (!f.exists()) {
					System.out.println("Cannot find obj-source-file " + fn);
					continue;
				}

				if (!(fn.toLowerCase().endsWith(".obj") && !fn.startsWith("."))) {
					System.out.println("Invalid obj-source-filename: " + fn);
					continue;
				}

				validationFlags |= Flag_SourceFound;
				sourceFiles.add(fn);
			}
		}

		if ((validationFlags & (Flag_TargetFound | Flag_SourceFound)) == 0) {
			System.out.println("Invalid argument input.");
			return;
		}

		System.out.println("Parsing OBJ-Files");
		LinkedList<OBJContainer> objs = new LinkedList<OBJContainer>();
		for (String fn : sourceFiles) {
			try {
				objs.add(OBJParser.parse(fn, conversionFlags));
			} catch (Exception e) {
				System.out.println("Cannot parse OBJ-file '" + fn + "'.");
				e.printStackTrace();
			}
		}

		System.out.println("Parsed " + objs.size() + " files.");

		// Basic binary file layout
		//
		// Foreach model:
		// <number_position_components:1>
		// <number_normal_components:1>
		// <number_texcoord_components:1>
		// <length_of_interleaved_buffer:4>
		// <interleaved_buffer:(length_of_interleaved_buffer * sizeof(float))>
		// <length_of_index_buffer:4>
		// <index_buffer:(length_of_index_buffer * sizeof(short))>

		if (objs.size() > 0) {
			System.out.println("Writing semodel-file to file: "
									   + targetFilename);
			long                    index      = 0;
			HashMap<String, String> modelIndex = new HashMap<String, String>();

			// Delete if a file with the given name already exists so that we
			// can create a new.
			File f = new File(targetFilename);
			if (f.exists())
				f.delete();

			BufferedWriter twr = null;
			try {
				boolean writeModelsBinary = (conversionFlags & OBJConversionFlags.WritePlainText.flag()) == 0;

				if (writeModelsBinary) {
					DataOutputStream bwr = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(f)));

					for (OBJContainer c : objs) {
						modelIndex.put(c.getName(), Integer.toString((int) index));

						bwr.write((byte) c.getVertexPositionComponentCount());
						bwr.write((byte) c.getVertexNormalComponentCount());
						bwr.write((byte) c.getVertexTexCoordComponentCount());

						int l = c.getInterleavedBuffer().length * (Float.SIZE / 8);
						bwr.writeInt(l);

						for (float fl : c.getInterleavedBuffer())
							bwr.writeFloat(fl);

						l = c.getIndexBuffer().length * (Short.SIZE / 8);
						bwr.writeInt(l);

						for (short s : c.getIndexBuffer())
							bwr.writeShort(s);

						// Register model index.
						// Add 2*sizeof(int) + 3*sizeof(Byte) for the meta
						// information of
						// each model!
						// Getting the buffer position equals iteratively adding:
						// index += 2 * Integer.SIZE + 3 * Byte.SIZE +
						// c.getInterleavedBuffer().length * Float.SIZE +
						// c.getIndexBuffer().length * Short.SIZE;s
						index = bwr.size();
					}

					bwr.close();
				} else {
					BufferedWriter plainTextOutput = new BufferedWriter(new FileWriter(f));

					index = 0;
					for (OBJContainer c : objs) {
						modelIndex.put(c.getName(), Integer.toString((int) (index++)));

						plainTextOutput.write(String.format("model:%s {%n", c.getName()));

						plainTextOutput.write(String.format("vertex-count:%d%n", c.getVertexPositionComponentCount()));
						plainTextOutput.write(String.format("normal-count:%d%n", c.getVertexNormalComponentCount()));
						plainTextOutput.write(String.format("texcoord-count:%d%n", c.getVertexTexCoordComponentCount()));

						int k = 0;
						int l = c.getInterleavedBuffer().length * (Float.SIZE / 8);
						plainTextOutput.write(String.format("vertices:%d {%n", l));
						for (float fl : c.getInterleavedBuffer())
							plainTextOutput.write(String.format(Locale.UK, "%s%.12f", (k++ == 0 ? "" : ","), fl));
						plainTextOutput.write(String.format("%n}%n"));

						k = 0;
						l = c.getIndexBuffer().length * (Short.SIZE / 8);
						plainTextOutput.write(String.format("indices:%d {%n", l));
						for (short s : c.getIndexBuffer())
							plainTextOutput.write(String.format("%s%d", (k++ == 0 ? "" : ","), s));
						plainTextOutput.write(String.format("%n}%n"));

						plainTextOutput.write(String.format("%n}%n"));
					}

					plainTextOutput.close();
				}

				// Write model index
				System.out.println("Writing semodel.index-file.");
				File indexFile = new File(targetFilename + ".index");
				twr = new BufferedWriter(new FileWriter(indexFile));

				twr.write(String.format("Binary:%s%n", Boolean.toString(writeModelsBinary)));
				for (Entry<String, String> pair : modelIndex.entrySet())
					twr.write(String.format("%s,%s%n", pair.getKey(), pair.getValue()));

				twr.close();

				System.out.println("Success");
			} catch (IOException e) {
				e.printStackTrace();
			}
		} else {
			System.out
					.println("Creating .semodel-File failed, since no OBJ-files were processable.");
		}

	}
}
