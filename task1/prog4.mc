{
    int sqrt(int N){
        for(int i : 1,N){
            if(i*i>=N) {
                return i;
            }
        }
        return 1;
    }

    int main(){
        int count = 0, lim=100000;
        for(int x : -sqrt(lim),sqrt(lim)){
            for(int y : -sqrt(lim), sqrt(lim)){
                if(x*x + y*y <=lim){
                    count = count + 1;
                }
            }
        } 

        STDOUT << count;

        return 0;
    }
}