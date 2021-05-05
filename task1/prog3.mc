{
    
    bool isPrime(int N){
        for( int i : 2,(N/i+1) ){
            if(N % i == 0) {
                return False;
            }
        }
        return True;
    }

    int main(){
        int N = 10;
        
        int sum = 0;
        for (int i : 2,N) {
            if(isPrime(i)) {
                sum = sum + i;
            }
        }

        STDOUT << sum;
        return 0;
    }
}