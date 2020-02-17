void init_clock_smart(long freq){
    long freq_base = 40000000;
    long scale = 0;
    while (freq_base/((pow(8,scale))*freq) > pow(2,16)){
        scale ++;
    }
    T3CONbits.TCKPS = scale;
    PR3 = freq_base/(pow(8,scale)*freq);
    T3CONbits.TON = 1;
}
