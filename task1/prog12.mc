{
    int getWordLen()[file input, int sz, int cur_pos]{
        char cur_char;
        int len = 0;
        while(cur_pos!=sz){
            input >> cur_char;
            len = len + 1;
            if(cur_char==' ' or cur_char=='\n'){
                break;
            }
        }
        return len;
    }

    void maxWordLenAndCount()[file input, int sz, int cur_pos, int max, int count]{
        max = 0;
        count = 0;
        while(cur_pos!=sz){
            int wordLen = getWordLen()[input, sz, cur_pos];
            if(wordLen>max) {
                max = wordLen;
            }
            count = count + 1;
        }
    }


    int sz=0, cur_pos=0;
    file input("in.txt", 'r')[sz, cur_pos];
    int max=0, count=0;

    maxWordLenAndCount()[input, sz, cur_pos, max, count];
    cur_pos = 0;

    char words[count][max+1];
    int unique_words = 0;

    int readWord()[file input, int sz, int cur_pos, char word[max+1]]{
        int len = 0;
        while(cur_pos!=sz){
            input >> word[len];
            if(word[len]==' ' or word[len]=='\n'){
                break;
            }
            len = len + 1;
        }
        word[len] = '\0';
        return len;
    }

    int addWord(int word_len, int max_word_len)[char word[max+1]]{
        bool is_same = False;
        for(int i : 0,unique_words){
            is_same = True;
            for(int j : 0, max_word_len){
                if(words[i][j] != word[j]){
                    is_same= False;
                    break;
                }
                if(word[j]=='\0'){
                    break;
                }
            }
            if(is_same){
                return unique_words;
            }
        }
        for(int i : 0, word_len){
            words[unique_words][i] = word[i];
        }
        unique_words = unique_words+1;
        return unique_words;
    }

    char word[max+1];
    cur_pos = 0;
    while(cur_pos!=sz){
        int word_len = readWord()[input, sz, cur_pos, word];
        addWord(word_len, max+1)[word];
    }

    for(int i : 0,unique_words){
        for(int j : 0, max+1){
            STDOUT << words[i][j];
            if(words[i][j]=='\0'){
                break;
            }
        }
        STDOUT << '\n';
    }
}