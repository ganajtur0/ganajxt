// TODO : 
//	- make file extension function universal
//	- write compile function
//	- fix main

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;

#define MIN(x, y) (x < y ? x : y)
#define error(x) fprintf(stderr, "%s\n", x)
#define TOLOWER(x) (isalpha(x) ? (x | 0x20) : x)

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

// A dynamic wchar_t struct and functions
typedef
struct {
    wchar_t *string;
    int length;
    int allocated;
} DynWchar;

DynWchar
new_wchar(void) {
    DynWchar dw;
    dw.string = calloc(64, sizeof(wchar_t));
    dw.length = 0;
    dw.allocated = 64;
    return dw;
}

int
wchar_append(DynWchar *dw, wchar_t wc) {
    if (dw->length == dw->allocated) {
	dw->allocated = (dw->allocated << 1);
	if (!reallocarray(dw->string, dw->allocated, sizeof(wchar_t))) {
	    error("Could not reallocate wchar_t array");
	    return 1;
	}
    }
    dw->string[(dw->length)++] = wc;
    return 0;
}

// This linked list is gonna have a function
// to insert the new element in alphabetical order

typedef
struct listitem {
    struct {
	char key[10];
	DynWchar value;
    } item;
    struct listitem *next;
} ListItem;

typedef
struct ctx {
    ListItem *first; // points to the first element of the itemlist
    ListItem *last; // points to the last element of the itemlist
} Ctx;

ListItem *
new_listitem(char key[10], DynWchar value) {
    ListItem *listitem = malloc(sizeof(ListItem));
    if (!listitem) {
	error("Allocation failed!\n");
	return NULL;
    }
    strncpy(listitem->item.key, key, strlen(key));
    listitem->item.value = value;
    return listitem;
}

void
ListItem_print( ListItem *li ) {
    printf("[%s]\n%ls\n", li->item.key, li->item.value.string);
}

// returns true if a should come
// after b in alphabetical order
static inline bool
gt(char a[10], char b[10]) {

    int a_len = strlen(a), b_len = strlen(b);

    bool result = a_len > b_len;

    int len = MIN(a_len, b_len);

    for ( int i = 0; i<len; ++i ) {
	if (TOLOWER(a[i]) < TOLOWER(b[i]))
	    return false;
    }
    return result;
}

void
list_free(ListItem *first) {
    ListItem *iter = first;
    while (iter != NULL) {
	ListItem *tmp = iter;
	iter = iter->next;
	free(iter->item.value.string);
	free(iter);
    }
}

void
list_insert(Ctx *ctx, char key[10], DynWchar value) {
    ListItem *new_item = new_listitem(key, value);
    if (!new_item) {
	list_free(ctx->first);
	exit(1);
    }
    if (ctx->first == NULL) {
	ctx->first = new_item;
	ctx->last  = new_item;
	new_item->next = NULL;
	return;
    }
    ListItem *iter = ctx->first;
    ListItem *tmp;
    /*
    while (iter->next != NULL) {
	tmp = iter;
	iter = iter->next;
	if (!gt(new_item->item.key, iter->item.key)) {
	    iter = tmp;
	    break;
	}
    }
    */
    while (iter->next != NULL
	&& gt(new_item->item.key, iter->item.key))
	    iter = iter->next;

    // we're inserting at the end
    if (iter->next == NULL) {
	iter->next = new_item;
	new_item->next = NULL;
	ctx->last = new_item;
	return;
    }
    tmp = iter->next;
    iter->next = new_item;
    new_item->next = tmp;
}

void
list_print(Ctx *ctx) {
    ListItem *iter = ctx->first;
    while (iter != NULL) {
	putchar('\n');
	ListItem_print(iter);
	iter = iter->next;
    }
}

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
decompile(char *gxt_filename, char *txt_filename) {

    FILE *gxt = fopen(gxt_filename, "r");
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

		    if ( fputwc(gtaiii_chartable[word-0x20], out) == WEOF )
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

char
get_prog_state (char *arg_1) {
    
    if (arg_1[0] != '-') {
	return 0;
    }
    return arg_1[1];
}

void
print_usage(char *prg_name) {
	printf("Usage: %s -[d|c] file_to_convert [output_file_name]\n", prg_name);
	printf("Use -d to [d]ecompile\n");
	printf("Use -c to [c]compile\n");
}

int
main( int argc, char *argv[] ) {

    setlocale(LC_ALL, "");

    char *txt_filename, prog_state;

    if ( argc < 3 ) {
	print_usage(argv[0]);
	return 0;
    }

    if ((prog_state = get_prog_state(argv[1])) == 0) {
	print_usage(argv[0]);
	return 0;
    }

    /*
    // if the user does not give a filename, we create it
    // from the given .gxt file's basename
    else if ( argc == 3 ) {
	if ( filename_with_txt_extension(argv[2], &txt_filename) ) {
	    fprintf(stderr, "Error when trying to create new filename!\n");
	    return 1;
	}
    }
    else if ( argc == 4 ) {
	txt_filename = argv[3];
    }

    if (decompile(argv[2], txt_filename))
	error("Couldn't decompile file");
    */

    // Testing the list
    char item1_key[10], item2_key[10], item3_key[10];
    DynWchar item1_value = new_wchar(),
	     item2_value = new_wchar(),
	     item3_value = new_wchar();

    strncpy(item1_key, "1000", 5);
    strncpy(item2_key, "TAPE", 5);
    strncpy(item3_key, "AUNT", 5);

    printf("AUNT > TAPE: %s\n", gt(item3_key, item2_key) ? "true" : "false");

    wchar_append(&item1_value, L'B');
    wchar_append(&item1_value, L'l');
    wchar_append(&item1_value, L'ö');
    wchar_append(&item1_value, L'd');

    wchar_append(&item2_value, L'B');
    wchar_append(&item2_value, L'l');
    wchar_append(&item2_value, L'ö');
    wchar_append(&item2_value, L'd');

    wchar_append(&item3_value, L'B');
    wchar_append(&item3_value, L'l');
    wchar_append(&item3_value, L'ö');
    wchar_append(&item3_value, L'd');
/*
    item1_value = malloc(sizeof(wchar_t) * 10);
    wcscpy(item1_value, L"Blöd1 Kuh");

    item2_value = malloc(sizeof(wchar_t) * 10);
    wcscpy(item2_value, L"Blödt Kuh");

    item3_value = malloc(sizeof(wchar_t) * 10);
    wcscpy(item3_value, L"Blöda Kuh");
*/
    Ctx c;
    c.first = NULL; c.last = NULL;

    list_insert(&c, item1_key, item1_value);
    list_insert(&c, item2_key, item2_value);
    list_insert(&c, item3_key, item3_value);

    list_print(&c);

    return 0;
}
