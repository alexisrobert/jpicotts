package org.alexis.jpicotts;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

public class PicoTTS {
	public PicoTTS(String ta_filename, String sg_filename) throws PicoTTSException {
		System.loadLibrary("jpico");
		this.setup(ta_filename, sg_filename);
	}

	private native void setup(String ta_filename, String sg_filename) throws PicoTTSException;
	public native byte[] say(String text) throws PicoTTSException;

	public void say_audio(String text) throws PicoTTSException, LineUnavailableException {
		AudioFormat format = new AudioFormat(16000, 16, 1, true, false);
		DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
		if (!AudioSystem.isLineSupported(info)) {
			throw new PicoTTSException("The sound system doesn't support output format");
		}

		SourceDataLine line;
		line = (SourceDataLine) AudioSystem.getLine(info);
		line.open(format);
		line.start();

		byte[] data = this.say(text);
		line.write(data, 0, data.length);

		line.stop();
		line.close();
	}
}
