package bees;

public class Main {

    private static int NUM_BEES = 10;
    static Pot pot = new Pot();
    private static Thread[] bees = new Thread[NUM_BEES];



    public static void main(String[] args){
        Thread bear = new Thread(new Bear(pot));
        bear.start();

        for(int i = 0; i < NUM_BEES; i++){
            bees[i] = new Thread(new Honeybees(pot, i));
            bees[i].start();
        }

        try{
            for(int i = 0; i < NUM_BEES; i++){
            bees[i].join();
        }
        bear.join(); 
        }catch(InterruptedException e){

        }
    }
     
}
