package org.alexis.jpicotts;

public class PicoTTS {
	public PicoTTS(String ta_filename, String sg_filename) throws PicoTTSException {
		System.loadLibrary("jpico");
		this.setup(ta_filename, sg_filename);
	}
	
	private native void setup(String ta_filename, String sg_filename) throws PicoTTSException;
	public native byte[] say(String text) throws PicoTTSException;
}
