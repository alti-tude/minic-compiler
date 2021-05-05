{   
    int pow(int base, int exp){
        if (exp==0) {
            return 1;
        } else {    
            return base*pow(base, exp-1);
        }
    }

    void main(){
        int N = 3, k = 2;
        int sum = 0;
        
        for(int i : 1,N) {
            sum = sum + pow(i,k);
        }

        STDOUT << sum;
        STDOUT << '\n';
    }
}