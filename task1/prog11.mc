{
    int count[26], sz=0, cur_pos=0;
    file input("in.txt", 'r')[sz, cur_pos];
    char cur_char;
    
    while(cur_pos!=sz){
        input >> cur_char;
        uint idx = cur_char - 'a';
        count[idx] = count[idx] + 1;
    }

    STDOUT << count;
    STDOUT << '\n';
}