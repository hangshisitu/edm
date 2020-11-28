package cn.edm.util;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

import au.com.bytecode.opencsv.CSVReader;
import au.com.bytecode.opencsv.CSVWriter;


public class Writers {

	public static void txt(String bom, String charset, InputStream input, OutputStream output) throws IOException {
		BufferedReader reader = null;
		PrintWriter writer = null;
		try {
			if (bom.equals("UTF-8")) {
				Streams streams = BOM.charset(input);
				reader = new BufferedReader(new InputStreamReader(streams.getInputStream(), "UTF-8"));
			} else {
				reader = new BufferedReader(new InputStreamReader(input, bom));
			}
			writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(output, charset)));
			String line = null;
			while ((line = reader.readLine()) != null) {
				writer.write(line);
				writer.write("\r\n");
			}
			writer.close();
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
		}
	}
	
	public static void csv(String bom, String charset, InputStream input, OutputStream output) throws IOException {
		CSVReader reader = null;
		CSVWriter writer = null;
		try {
			if (bom.equals("UTF-8")) {
				//Streams streams = BOM.charset(input);
				reader = new CSVReader(new BufferedReader(new InputStreamReader(input, "UTF-8")));
			} else {
				reader = new CSVReader(new BufferedReader(new InputStreamReader(input, bom)));
			}
			
			writer = new CSVWriter(new BufferedWriter(new OutputStreamWriter(output, charset)), ',', CSVWriter.NO_QUOTE_CHARACTER);
			
			String[] nextLine = null;
			while ((nextLine = reader.readNext()) != null) {
				writer.writeNext(nextLine);
			}
			writer.close();
		} finally {
			try {
				if (reader != null) {
					reader.close();
				}
			} catch (IOException e) {
			}
		}
	}
	
	}
