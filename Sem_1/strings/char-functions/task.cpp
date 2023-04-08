bool isalpha(unsigned char c) {
    return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));
}

unsigned char tolower(unsigned char c) {
    return ((c >= 65 && c <= 90) ? char(c + 32) : char(c));
}

