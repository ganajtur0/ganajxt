#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;

#define error(x) fprintf(stderr, "%s\n", x)

wchar_t gtaiii_chartable[] = {L' ', L'!', L'△', L'#', L'$', L'%', L'&', L'\'', L'(', L')',
			      L'*', L'+', L',', L'-', L'.', L'/', L'0', L'1', L'2', L'3',
			      L'4', L'5', L'6', L'7', L'8', L'9', L':', L';', L'◀', L'=',
			      L'▶', L'?', L'™', L'A', L'B', L'C', L'D', L'E', L'F', L'G',
			      L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q',
			      L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'[',
			      L'\\', L']', L'^', L'°', L'`', L'a', L'b', L'c', L'd', L'e',
			      L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o',
			      L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y',
			      L'z', 0x7B, L'◯', 0x7D, L'~', 0x7F, L'À', L'Á', L'Â', L'Ä',
			      L'Æ', L'Ç', L'È', L'É', L'Ê', L'Ë', L'Ì', L'Í', L'Î', L'Ï',
			      L'Ò', L'Ó', L'Ô', L'Ö', L'Ù', L'Ú', L'Û', L'Ü', L'ß', L'à',
			      L'á', L'â', L'ä', L'æ', L'ç', L'è', L'é', L'ê', L'ë', L'ì',
			      L'í', L'î', L'ï', L'ò', L'ó', L'ô', L'ö', L'ù', L'ú', L'û',
			      L'ü', L'Ñ', L'ñ', L'¿', L'¡', 0xB1};

int
filename_with_txt_extension(char *filename, char **result) {

    char *tmp = strdup(filename);

    char *last_dot = strrchr(tmp, '.');

    if ( last_dot == NULL || last_dot == filename ) return 1;

    *last_dot = '\0';
    
    int tmp_len = strlen(tmp);

    *result = malloc( tmp_len + 5 );

    int i = 0; char *iter = *result;
    for ( ;
	  i<tmp_len && tmp[i] != '\0';
	  (*(iter++)) = tmp[i++]
	);

    strncpy(iter, ".txt", 4);

    (*result)[tmp_len + 4] = '\0';

    free(tmp);

    return 0;

}

static inline int
fail(char *error, FILE *gxt, FILE *out) {
    error(error);
    fclose(gxt);
    fclose(out);
    return 1;
}

int
main( int argc, char *argv[] ) {

    char *txt_filename;

    if ( argc < 2 ) {
	printf("Usage: %s file_to_convert [output_file_name]\n", argv[0]);
	return 0;
    }
    else if ( argc == 2 ) {
	if ( filename_with_txt_extension(argv[1], &txt_filename) ) {
	    fprintf(stderr, "Error when trying to create new filename!\n");
	    return 1;
	}
    }
    else if ( argc == 3 ) {
	txt_filename = argv[2];
    }

    setlocale(LC_ALL, "");

    FILE *gxt = fopen(argv[1], "r");
    if (gxt == NULL) {
	    error("Could not open file!");
	    return 1;
    }

    FILE *out = fopen(txt_filename, "w, ccs=UTF-8");

    if (out == NULL) {
	    error("Could not open file!");
	    return 1;
    }

    fseek(gxt, 4, SEEK_CUR);
    
    DWORD tkey_block_size;
    fread(&tkey_block_size, sizeof(DWORD), 1, gxt);

    DWORD tdat_entry_offset;
    char key[8];

    while ( ftell(gxt) < tkey_block_size ) {

	    fread(&tdat_entry_offset, sizeof(DWORD), 1, gxt);
	    fread(key, sizeof(char), 8, gxt);

	    fputwc(L'[', out);

	    for ( int i = 0; i<8; ++i) {

		if ( key[i] == '\0' ) break;

		wchar_t tmp_wc;
		mbtowc(&tmp_wc, &key[i], 1);
		fputwc(tmp_wc, out);

	    }

	    fwprintf(out, L"]\n");


	    long save_state = ftell(gxt);

	    fseek(gxt, tkey_block_size + 16 + tdat_entry_offset, SEEK_SET);

	    WORD word;
	    fread(&word, 2, 1, gxt);

	    while ( word != 0x0000 ) {

		    if ( fputwc(gtaiii_chartable[word - 0x20], out) == WEOF )
			    return fail("Could not write wchar_t to output file!\n",
					out, gxt);

		    fread(&word, 2, 1, gxt);
	    }

	    fputwc(L'\n', out);
	    fputwc(L'\n', out);

	    fseek(gxt, save_state, SEEK_SET);


    }

    fclose(gxt);
    fclose(out);

    return 0;
}
