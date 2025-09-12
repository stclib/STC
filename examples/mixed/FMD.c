#include <stdio.h>
#include <time.h>
#include <stc/cstr.h>
#include <c11/fmt.h>

typedef struct {
    cstr fileName;
    cstr directory;
    isize size;
    time_t lastWriteTime;
}  FileMetaData;

enum FMDActive {FMD_fileName, FMD_directory, FMD_size, FMD_lastWriteTime, FMD_LAST};
typedef struct { enum FMDActive activeField; bool reverse; } FMDVectorSorting;

int FileMetaData_cmp(const FMDVectorSorting*, const FileMetaData*, const FileMetaData*);
void FileMetaData_drop(FileMetaData*);

#define T FMDVector, FileMetaData, (c_keyclass | c_no_clone)
#define i_aux FMDVectorSorting
#define i_cmp(x, y) FileMetaData_cmp(&self->aux, x, y)
#include <stc/stack.h>
// --------------

int FileMetaData_cmp(const FMDVectorSorting* aux, const FileMetaData* a, const FileMetaData* b) {
    int dir = aux->reverse ? -1 : 1;
    switch (aux->activeField) {
        case FMD_fileName: return dir*cstr_cmp(&a->fileName, &b->fileName);
        case FMD_directory: return dir*cstr_cmp(&a->directory, &b->directory);
        case FMD_size: return dir*c_default_cmp(&a->size, &b->size);
        case FMD_lastWriteTime: return dir*c_default_cmp(&a->lastWriteTime, &b->lastWriteTime);
        default:;
    }
    return 0;
}

void FileMetaData_drop(FileMetaData* fmd) {
    cstr_drop(&fmd->fileName);
    cstr_drop(&fmd->directory);
}

int main(void) {
    FMDVector vec = c_make(FMDVector, {
        {cstr_from("WScript.cpp"), cstr_from("code/unix"), 3624, 123567},
        {cstr_from("CanvasBackground.cpp"), cstr_from("code/unix/canvas"), 38273, 12398},
        {cstr_from("Brush_test.cpp"), cstr_from("code/tests"), 67236, 7823},
    });

    vec.aux.reverse = true;
    for (c_range_t(enum FMDActive, field, FMD_LAST)) {
        vec.aux.activeField = field;
        FMDVector_sort(&vec);

        for (c_each_item(e, FMDVector, vec)) {
            fmt_println("{:30}{:30}{:10}{:10}",
                        cstr_str(&e->fileName), cstr_str(&e->directory),
                        e->size, e->lastWriteTime);
        }
        puts("");
    }
    FMDVector_drop(&vec);
}
