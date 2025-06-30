void kernel_main(void) {
    int a = 0;

    char* video = (char*)0xb8000;

    *video = 'G';
}