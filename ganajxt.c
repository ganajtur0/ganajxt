// TODO : 
//	- finish writing compile function

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
dwc_new(void) {
    DynWchar dw;
    dw.string = calloc(64, sizeof(wchar_t));
    dw.length = 0;
    dw.allocated = 64;
    return dw;
}

int
dwc_append(DynWchar *dw, wchar_t wc) {
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

void
dwc_delete(DynWchar *dwc) {
    free(dwc->string);
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
    enum {
	KEY,
	VALUE,
	COMMENT,
	NEWLINE,
	UNDEFINED
    } state;
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
	free(tmp->item.value.string);
	free(tmp);
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
change_file_extension(char *filename, const char *ext, char **result) {

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

    strncpy(iter, ext, 4);

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

int
compile(Ctx *c, const char *txt_filename, const char *gxt_filename) {
    
    FILE *txtfile  = fopen(txt_filename, "r");
    if ( txtfile == NULL ) {
	error("Could not open .txt file!");
	return 1;
    }

    FILE *gxtfile = fopen(gxt_filename, "wb");
    if ( gxtfile == NULL ) {
	error("Could not open .gxt file");
	return 1;
    }

    wchar_t wc;

    char key_buffer[10] = {0};
    int key_buffer_size = 0;

    DynWchar dwc = dwc_new();

    while ( (wc = fgetwc(txtfile)) != WEOF ) {
	switch (wc) {
	case L'[':
	    if ( c->state == VALUE )
		printf("%d, %ls\n", dwc.length, dwc.string);
	    memset(key_buffer, 0, 10);
	    key_buffer_size = 0;
	    c->state = KEY;
	    break;
	case L']':
	    printf("%s\n", key_buffer);
	    c->state = VALUE;
	    break;
	case L'{':
	    c->state = COMMENT;
	    break;
	case L'}':
	    c->state = UNDEFINED;
	    break;
	default:
	    switch (c->state) {
	    case KEY:
		key_buffer[key_buffer_size++] = (char)wc;
		break;
	    case VALUE:
	    case UNDEFINED:
	    	dwc_append(&dwc, wc);
		break;
	    default:
		break;
	    }
	}	
    }

    fclose(txtfile);
    fclose(gxtfile);

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

    char *new_filename, prog_state;

    if ( argc < 3 ) {
	print_usage(argv[0]);
	return 0;
    }

    if ((prog_state = get_prog_state(argv[1])) == 0) {
	print_usage(argv[0]);
	return 0;
    }

    switch (prog_state) {

	// decompile file
	case 'd':
	// the user did not provide an output filename
	if ( argc == 3 ) {
	    if ( change_file_extension( argv[2],
					".txt",
					&new_filename) ) {
		error("Could not create filename with txt extension!");
		return 1;
	    }
	}
	else 
	    new_filename = argv[3];

	if (decompile(argv[2], new_filename)) {
	    error("Error during decompilation process");
	    return 1;
	}

	break;

	// compile file
	case 'c':
	// the user did not provide an output filename
	if ( argc == 3 ) {
	    if ( change_file_extension( argv[2],
					".gxt",
					&new_filename) ) {
		error("Could not create filename with txt extension!");
		return 1;
	    }
	}
	else 
	    new_filename = argv[3];

	Ctx c;
	c.first = NULL;
	c.last  = NULL;

	if (compile(&c, argv[2], new_filename)) {
	    error("Error during compilation process");
	    return 1;
	}

	break;

	default:
	break;
    }

    return 0;
}
