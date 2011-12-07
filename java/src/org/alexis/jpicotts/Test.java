package org.alexis.jpicotts;

import javax.sound.sampled.LineUnavailableException;

public class Test {
	public static void main(String[] args) {
		try {
			PicoTTS pico = new PicoTTS("/home/alexis/picotts/fr-FR_ta.bin", "/home/alexis/picotts/fr-FR_nk0_sg.bin");
			
			pico.say_audio("J'ai pas une voix horrible !");
		} catch (PicoTTSException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (LineUnavailableException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
