#include "src/lea.h"

#include <stdio.h>

//
unsigned char mk[16] = { 
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

unsigned char N[32] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
	0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

unsigned char A[32] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
	0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

unsigned char pt[32] = { 0, };

//lengths in bytes
int NlenCCM[4] = { 7, 8, 12, 13 };
int NlenGCM[6] = { 10, 12, 14, 16, 25, 32 };

int AlenCCM[5] = { 0, 7, 16, 24, 32 };
int AlenGCM[5] = { 0, 7, 16, 24, 32 };

int PlenCCM[5] = { 0, 8, 16, 24, 32 };
int PlenGCM[5] = { 0, 8, 16, 24, 32 };

unsigned char ct[32];


unsigned char T[16];

//mklen fixed to 16
int mklen = 16;
//Tlen fixed to 16
int Tlen = 16;

/*
void TestCCM(){
	int i, j, k, l;
	LEA_KEY lea_key;
	FILE *fout;

	fout = fopen("CCMTV.txt", "w");

	lea_set_key(&lea_key, mk, mklen);

	for (i = 0; i < 4; i++){//Nlen
		for (j = 0; j < 5; j++){//Alen
			for (k = 0; k < 5; k++){//Plen
				lea_ccm_enc(ct, T, pt, PlenCCM[k], Tlen, N, NlenCCM[i], A, AlenCCM[j], &lea_key);
				fprintf(fout, "DATA[%2d%2d%2d]\n", i, j, k);
				fprintf(fout, "CT\n");
				for (l = 0; l < PlenCCM[k]; l++) {
					fprintf(fout, "%02x", ct[l]);
					if (l % 16 == 15) fprintf(fout, "\n");
				}
				if (PlenCCM[k] % 16) fprintf(fout, "\n");
				fprintf(fout, "T\n");
				for (l = 0; l < Tlen; l++) fprintf(fout, "%02x", T[l]);
				fprintf(fout, "\n");
			}
		}
	}

	fclose(fout);
}
*/

void TestCCM(){
	int i, j, k, l;
	LEA_KEY lea_key;
	FILE *fout;

	fout = fopen("CCMTV.txt", "w");

	lea_set_key(&lea_key, mk, mklen);

	for (i = 0; i < 4; i++){//Nlen
		for (j = 0; j < 5; j++){//Alen
			for (k = 0; k < 5; k++){//Plen
				lea_ccm_enc(ct, T, pt, PlenCCM[k], Tlen, N, NlenCCM[i], A, AlenCCM[j], &lea_key);
				fprintf(fout, "%d%d%d\n", i, j, k);
//				fprintf(fout, "CT:");
				for (l = 0; l < PlenCCM[k]; l++) {
					fprintf(fout, "%02x", ct[l]);					
				}
				fprintf(fout, "\n");
//				fprintf(fout, "T:");
				for (l = 0; l < Tlen; l++) fprintf(fout, "%02x", T[l]);
				fprintf(fout, "\n");
			}
		}
	}

	fclose(fout);
}

/*
void TestGCM(){

	int i, j, k, l;

	LEA_GCM_CTX gcmctx;

	FILE *fout;

	fout = fopen("GCMTV.txt", "w");
	lea_gcm_init(&gcmctx, mk, mklen);

	for (i = 0; i < 6; i++){//Nlen
		for (j = 0; j < 5; j++){//Alen
			for (k = 0; k < 5; k++){//Plen
				
				lea_gcm_set_ctr(&gcmctx, N, NlenGCM[i]);
				lea_gcm_set_aad(&gcmctx, A, AlenGCM[j]);
				lea_gcm_enc(&gcmctx, ct, pt, PlenGCM[k]);
				lea_gcm_final(&gcmctx, T, Tlen);

				fprintf(fout, "DATA[%2d%2d%2d]\n", i, j, k);
				fprintf(fout, "CT\n");
				for (l = 0; l < PlenGCM[k]; l++) {
					fprintf(fout, "%02x", ct[l]);
					if (l % 16 == 15) fprintf(fout, "\n");
				}
				if (PlenGCM[k] % 16) fprintf(fout, "\n");
				fprintf(fout, "T\n");
				for (l = 0; l < Tlen; l++) fprintf(fout, "%02x", T[l]);
				fprintf(fout, "\n");

			}
		}
	}

	fclose(fout);

}
*/

void TestGCM(){

	int i, j, k, l;

	LEA_GCM_CTX gcmctx;

	FILE *fout;

	lea_gcm_init(&gcmctx, mk, mklen);

	fout = fopen("GCMTV.txt", "w");
	

	for (i = 0; i < 6; i++){//Nlen
		for (j = 0; j < 5; j++){//Alen
			for (k = 0; k < 5; k++){//Plen
				
//				lea_gcm_init(&gcmctx, mk, mklen);
				lea_gcm_set_ctr(&gcmctx, N, NlenGCM[i]);
				lea_gcm_set_aad(&gcmctx, A, AlenGCM[j]);
				lea_gcm_enc(&gcmctx, ct, pt, PlenGCM[k]);
				lea_gcm_final(&gcmctx, T, Tlen);

				fprintf(fout, "%d%d%d\n", i, j, k);
				fprintf(fout,"%d,%d,%d\n", NlenGCM[i], AlenGCM[j], PlenGCM[k]);
				//				fprintf(fout, "CT:");
				for (l = 0; l < PlenGCM[k]; l++) {
					fprintf(fout, "%02x", ct[l]);
				}
				fprintf(fout, "\n");
				//				fprintf(fout, "T:");
				for (l = 0; l < Tlen; l++) fprintf(fout, "%02x", T[l]);
				fprintf(fout, "\n");

			}
		}
	}

	fclose(fout);

}



int main()
{
	init_simd();
	TestCCM();
	TestGCM();
	return 0;
}
	
	



	

