#include <picoapi.h>
#include <picodefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "org_alexis_jpicotts_PicoTTS.h"

#define PICO_MEM_SIZE 2500000
#define MAX_OUTBUF_SIZE 128
#define PICO_VOICE_NAME "PicoVoice"

#define PICOTTS_EXCEPTION_CLASS "org/alexis/jpicotts/PicoTTSException"

pico_System     picoSystem          = NULL;
pico_Char *     picoTaResourceName  = NULL;
pico_Char *		picoSgResourceName	= NULL;
pico_Engine     picoEngine          = NULL;

void raise_exception(JNIEnv *env, const char* text) {
	jclass exception = (*env)->FindClass(env, PICOTTS_EXCEPTION_CLASS);
	(*env)->ThrowNew(env, exception, text);
}

int load_data(JNIEnv *env, const char* ta_filename, const char* sg_filename) {
	pico_Resource   picoTaResource      = NULL;	
	pico_Resource   picoSgResource      = NULL;	

	picoTaResourceName  = (pico_Char *) malloc( PICO_MAX_RESOURCE_NAME_SIZE );
	picoSgResourceName  = (pico_Char *) malloc( PICO_MAX_RESOURCE_NAME_SIZE );

	pico_Status status = pico_loadResource(picoSystem, ta_filename, &picoTaResource);

	if (PICO_OK != status) {
		raise_exception(env, "Failed to load text analysis data");
		return 1;
	}

	status = pico_loadResource(picoSystem, sg_filename, &picoSgResource);

	if (PICO_OK != status) {
		raise_exception(env, "Failed to load sound generation data");
		return 1;
	}

	status = pico_getResourceName(picoSystem, picoTaResource, (char*)picoTaResourceName);

	if (PICO_OK != status) {
		raise_exception(env, "Failed to get resource name for text analysis data");
		return 1;
	}

	status = pico_getResourceName(picoSystem, picoSgResource, (char*)picoSgResourceName);

	if (PICO_OK != status) {
		raise_exception(env, "Failed to get resource name for sound generation data");
		return 1;
	}

	return 0;
}

JNIEXPORT void JNICALL Java_org_alexis_jpicotts_PicoTTS_setup
  (JNIEnv *env, jobject parent, jstring ta_filename, jstring sg_filename) {
	// INIT
	void *memory = malloc(PICO_MEM_SIZE);

	pico_Status status = pico_initialize(memory, PICO_MEM_SIZE, &picoSystem);
	if (PICO_OK != status) {
		raise_exception(env, "Failed to initialize Pico system");
		return;
	}

	pico_Char* voicename = (pico_Char*) PICO_VOICE_NAME;
	status = pico_createVoiceDefinition(picoSystem, voicename);

	if (PICO_OK != status) {
		raise_exception(env, "Failed creating voice definition");
		return;
	}

	// Load resource data
	const char* ta_filename_native = (*env)->GetStringUTFChars(env, ta_filename, NULL);
	const char* sg_filename_native = (*env)->GetStringUTFChars(env, sg_filename, NULL);
	if (load_data(env, ta_filename_native, sg_filename_native) != 0)
		return;
	(*env)->ReleaseStringUTFChars(env, ta_filename, ta_filename_native);
	(*env)->ReleaseStringUTFChars(env, sg_filename, sg_filename_native);

	// Add the text analysis resource
	status = pico_addResourceToVoiceDefinition(picoSystem, PICO_VOICE_NAME,
		picoTaResourceName);	

	if (PICO_OK != status) {
		raise_exception(env, "Failed to add the text analysis resource");
		return;
	}

	// Add the signal generation resource
	status = pico_addResourceToVoiceDefinition(picoSystem, PICO_VOICE_NAME,
		picoSgResourceName);		

	if (PICO_OK != status) {
		raise_exception(env, "Failed to add the signal generation resource");
		return;
	}

	// Initialize the engine
	status = pico_newEngine(picoSystem, voicename, &picoEngine);

	if (PICO_OK != status) {
		raise_exception(env, "Failed to load the engine");
		return;
	}
}

JNIEXPORT jbyteArray JNICALL Java_org_alexis_jpicotts_PicoTTS_say
  (JNIEnv *env, jobject parent, jstring text) {

	jbyte *output = NULL;
	size_t output_length = 0;

	const char *input = (*env)->GetStringUTFChars(env, text, NULL);
	int textRemaining = (*env)->GetStringLength(env, text)+1;

	printf("string=%s,length=%d\n", input, textRemaining-1);
	pico_Char *inp = (pico_Char*) input;
	jbyte *outBuffer = malloc(MAX_OUTBUF_SIZE/2);
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
				
				output = realloc(output, (output_length+((size_t)bytesReceived))*sizeof(short));
				memcpy((jbyte*)(output+output_length), outBuffer, bytesReceived);
				output_length = output_length+bytesReceived;
			}
		} while (PICO_STEP_BUSY == status);
	}	

	jbyteArray jni_output = (*env)->NewByteArray(env, output_length);
	(*env)->SetByteArrayRegion(env, jni_output, 0, output_length, output);

	free(output);
	free(outBuffer);
	(*env)->ReleaseStringUTFChars(env, text, input);

	return jni_output;
}
