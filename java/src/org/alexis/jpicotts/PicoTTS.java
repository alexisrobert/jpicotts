package org.alexis.jpicotts;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

public class PicoTTS {
	/**
	 * Initializes the TTS engine using data files given in parameters.
	 * 
	 * @param ta_filename text analysis file (like fr-FR_ta.bin)
	 * @param sg_filename sound generation file (like fr-FR_nk0_sg.bin)
	 * @throws PicoTTSException
	 */
	public PicoTTS(String ta_filename, String sg_filename) throws PicoTTSException {
		System.loadLibrary("jpicotts");
		this.setup(ta_filename, sg_filename);
	}

	private native void setup(String ta_filename, String sg_filename) throws PicoTTSException;
	
	/**
	 * Say the text given as parameter and returns a byte[] of
	 * raw sound samples
	 * 
	 * @param text Text to speak
	 * @return byte[] of raw sound samples (16kHz, 16bits, mono, signed, little endian)
	 * @throws PicoTTSException
	 */
	public native byte[] say_raw(String text) throws PicoTTSException;

	/**
	 * Say the text given as parameter on the default sound card
	 * using JavaSound API.
	 * 
	 * @param text Text to speak
	 * @throws PicoTTSException
	 * @throws LineUnavailableException
	 */
	public void say(String text) throws PicoTTSException, LineUnavailableException {
		AudioFormat format = new AudioFormat(16000, 16, 1, true, false);
		DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
		if (!AudioSystem.isLineSupported(info)) {
			throw new PicoTTSException("The sound system doesn't support output format");
		}

		SourceDataLine line;
		line = (SourceDataLine) AudioSystem.getLine(info);
		line.open(format);
		line.start();

		byte[] data = this.say_raw(text);
		line.write(data, 0, data.length);

		line.stop();
		line.close();
	}
}
