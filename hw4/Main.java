
public class Main {
    private static int NUM_BABY_BIRD = 10;
    static Dish dish = new Dish();

    private static Thread[] baby_birds = new Thread[NUM_BABY_BIRD];

    public static void main(String[] args){
        Thread papa_t = new Thread(new PapaBird(dish));
        papa_t.start();

        for(int i = 0; i < NUM_BABY_BIRD; i++){
            baby_birds[i] = new Thread(new BabyBird(dish, i));
            baby_birds[i].start();
        }
        try{
            for(int i = 0; i < NUM_BABY_BIRD; i++){
            baby_birds[i].join();
        }
        papa_t.join(); 
        }catch(InterruptedException e){

        }
    }
}
