{
    int main(){
        int N = 10;

        bool flag = False;
        for( int i : 2,(N/i+1) ){
            if(N % i == 0) {
                flag = True;
                STDOUT << 'N';
                return 0;
            }
        }
        if(not flag) {
            STDOUT << 'Y';
        }

        return 0;
    }
}