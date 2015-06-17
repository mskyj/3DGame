#include "Julius.h"

#include <iostream>

Recog *recog = NULL;
char voiceStr[128];
void* hMutex;
bool juliusFinishFlag = false;

bool JuliusInit()
{
	char** arg;
	arg = new char*[3];
	for( int i=0; i<3; i++)
		arg[i] = new char[64];
	memcpy_s( arg[0], 64, "\0", 1);
	memcpy_s( arg[1], 64, "-C\0", 3);
	memcpy_s( arg[2], 64, "data/test.jconf\0", 16);

	Jconf *jconf = j_config_load_args_new(3, arg);
	if (jconf == NULL) {
		std::cout << "Error @ j_config_load_args_new" << std::endl;
		return false;
	}

	// Recog: Top level instance for the whole recognition process
	// create recognition instance according to the jconf
	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL) {
		std::cout << "Error @ j_create_instance_from_jconf" << std::endl;
		return false;
	}

	// Regster callback
	callback_add(recog, CALLBACK_EVENT_SPEECH_READY, [](Recog *recog, void*) {
		std::cout << "<<< PLEASE SPEAK! >>>" << std::endl;
	}, NULL);

	callback_add(recog, CALLBACK_EVENT_SPEECH_START, [](Recog *recog, void*) {
		std::cout << "...SPEECH START..." << std::endl;
	}, NULL);

	callback_add(recog, CALLBACK_RESULT, [](Recog *recog, void*) {
		for (const RecogProcess *r = recog->process_list; r; r = r->next) {
			// èàóùÇ™ê∂Ç´ÇƒÇ»Ç¢Ç∆Ç´ÇÕîÚÇŒÇ∑
			if (! r->live) continue;
			// èàóùåãâ Ç™ìæÇÁÇÍÇ»Ç¢éûÇ‡îÚÇŒÇ∑
			if (r->result.status < 0) continue;

			WORD_INFO *winfo = r->lm->winfo;
			for (int n = 0; n < r->result.sentnum; ++n) {
				Sentence *s   = &(r->result.sent[n]);
				WORD_ID *seq = s->word;
				int seqnum   = s->word_num;
				for (int i = 0; i < seqnum; ++i) {
					std::cout << winfo->woutput[seq[i]];
					WaitForSingleObject(hMutex,INFINITE);
					memcpy_s( voiceStr, 128, winfo->woutput[seq[i]], 128 );
					ReleaseMutex(hMutex);
					if( !strcmp( winfo->woutput[seq[i]], "èIóπ" ) || juliusFinishFlag )
						j_close_stream(recog);
				}
			}
			std::cout << std::endl;
		}
	}, NULL);

	// Initialize audio input
	if (j_adin_init(recog) == FALSE) {
		return false;
	}

	// output system information to log
	j_recog_info(recog);

	// Open input stream and recognize
	switch (j_open_stream(recog, NULL)) {
		case  0: break; // success
		case -1: std::cout << "Error in input stream" << std::endl; return false;
		case -2: std::cout << "Failed to begin input stream" << std::endl; return false;
	}

	return true;
}

void JuliusLoopFunc(LPVOID pParam)
{
	// Recognition loop
	int ret = j_recognize_stream(recog);
	if (ret == -1) ExitThread(TRUE);
}

void JuliusClose()
{
	if( recog )
	{
		j_close_stream(recog);
		j_recog_free(recog);
	}
}