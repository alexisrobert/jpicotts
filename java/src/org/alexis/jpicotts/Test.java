package org.alexis.jpicotts;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class Test {
	public static void main(String[] args) {
		try {
			PicoTTS pico;
			pico = new PicoTTS("/home/alexis/picotts/fr-FR_ta.bin", "/home/alexis/picotts/fr-FR_nk0_sg.bin");
			FileOutputStream fos = new FileOutputStream("output.raw");
			
			System.out.println("lol");
			byte[] data = pico.say("Aéré");
			fos.write(data);
			fos.close();
			
		} catch (PicoTTSException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
