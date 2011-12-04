#include <picoapi.h>
#include <picodefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "org_alexis_jpicotts_PicoTTS.h"

#define PICO_MEM_SIZE 2500000
#define MAX_OUTBUF_SIZE 128
#define PICO_VOICE_NAME "PicoVoice"

#define TA_FILENAME "fr-FR_ta.bin"
#define SG_FILENAME "fr-FR_nk0_sg.bin"

pico_System     picoSystem          = NULL;
pico_Char *     picoTaResourceName  = NULL;
pico_Char *		picoSgResourceName	= NULL;
pico_Engine     picoEngine          = NULL;

int load_data() {
	pico_Resource   picoTaResource      = NULL;	
	pico_Resource   picoSgResource      = NULL;	

	picoTaResourceName  = (pico_Char *) malloc( PICO_MAX_RESOURCE_NAME_SIZE );
	picoSgResourceName  = (pico_Char *) malloc( PICO_MAX_RESOURCE_NAME_SIZE );

	pico_Status status = pico_loadResource(picoSystem, TA_FILENAME, &picoTaResource);

	if (PICO_OK != status) {
		fprintf(stderr, "Failed to load text analysis data\n");
		return 1;
	}

	status = pico_loadResource(picoSystem, SG_FILENAME, &picoSgResource);

	if (PICO_OK != status) {
		fprintf(stderr, "Failed to load sound generation data\n");
		return 1;
	}

	status = pico_getResourceName(picoSystem, picoTaResource, (char*)picoTaResourceName);

	if (PICO_OK != status) {
		fprintf(stderr, "Failed to get resource name for text analysis data\n");
		return 1;
	}

	status = pico_getResourceName(picoSystem, picoSgResource, (char*)picoSgResourceName);

	if (PICO_OK != status) {
		fprintf(stderr, "Failed to get resource name for sound generation data\n");
		return 1;
	}

	return 0;
}

JNIEXPORT void JNICALL Java_org_alexis_jpicotts_PicoTTS_setup
  (JNIEnv *env, jobject parent) {
	// INIT
	void *memory = malloc(PICO_MEM_SIZE);

	pico_Status status = pico_initialize(memory, PICO_MEM_SIZE, &picoSystem);
	if (PICO_OK != status) {
		fprintf(stderr, "Failed to initialize Pico system\n");
		return;
	}

	pico_Char* voicename = (pico_Char*) PICO_VOICE_NAME;
	status = pico_createVoiceDefinition(picoSystem, voicename);

	if (PICO_OK != status) {
		fprintf(stderr, "Failed creating voice definition\n");
		return;
	}

	// Load resource data
	if (load_data() != 0)
		return;

	// Add the text analysis resource
	status = pico_addResourceToVoiceDefinition(picoSystem, PICO_VOICE_NAME,
		picoTaResourceName);	

	// Add the signal generation resource
	status = pico_addResourceToVoiceDefinition(picoSystem, PICO_VOICE_NAME,
		picoSgResourceName);		

	// Initialize the engine
	status = pico_newEngine(picoSystem, voicename, &picoEngine);

	if (PICO_OK != status) {
		fprintf(stderr, "Failed to load the engine\n");
		return;
	}
}

JNIEXPORT void JNICALL Java_org_alexis_jpicotts_PicoTTS_say
  (JNIEnv *env, jobject parent, jstring text) {

	FILE *output = fopen("output.raw", "w");

	const jchar *input = (*env)->GetStringUTFChars(env, text, NULL);
	int textRemaining = (*env)->GetStringLength(env, text)+1;

	printf("string=%s,length=%d\n", input, textRemaining-1);
	pico_Char *inp = (pico_Char*) input;
	short *outBuffer = malloc(MAX_OUTBUF_SIZE/2);
	pico_Status status;

	pico_Int16 bytesSent, bytesReceived, outDataType;

	while(textRemaining) {
		status = pico_putTextUtf8(picoEngine, inp, textRemaining, &bytesSent);
		textRemaining -= bytesSent;
		inp += bytesSent;

		do {
			status = pico_getData(picoEngine, (void*) outBuffer,
					MAX_OUTBUF_SIZE-1, &bytesReceived, &outDataType);
			if (bytesReceived) {
				printf("receive %d bytes (type=%d)\n", bytesReceived, outDataType);
				
				size_t blah = (size_t)bytesReceived;
				fwrite(outBuffer, blah, 1, output);
			}
		} while (PICO_STEP_BUSY == status);
	}

	fclose(output);
	(*env)->ReleaseStringChars(env, text, input);
}
