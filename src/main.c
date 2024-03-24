#include "vm.h"

// begin extension demo
void myadd(forthvm_t *vm) {
    data_t a = vm_pop_ds(vm);
    data_t b = vm_pop_ds(vm);
    vm_push_ds(vm, a + b);
}

void load_ext(forthvm_t *vm) { vm_regfunc(vm, "__myadd__", myadd); }
// end extension demo

int main(int argc, char **argv) {
    forthvm_t vm;
    int ret;
    char *filename = "stdin";
    FILE *fin = stdin;
    if (argc > 1) {
        fin = fopen(argv[1], "r");
        if (fin == NULL) {
            fprintf(stderr, "Failed to open file: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        filename = argv[1];
    }
    vm_init(&vm, fin, stdout);
    // extensions must be loaded after initialization
    load_ext(&vm);
    vm_run(&vm);
    if (vm.ret == -2) {
        fprintf(stderr, "Assertion failed at %s:%d\n", filename, vm.linenum);
    } else if (vm.ret < 0) {
        fprintf(stderr, "VM error at %s:%d: %s\n", filename, vm.linenum,
                vm.errmsg);
    }
    return vm.ret;
}
