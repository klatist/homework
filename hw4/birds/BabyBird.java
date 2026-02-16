package birds;
import java.util.concurrent.*;
import java.lang.Runnable;
import java.util.Random;



public class BabyBird implements Runnable {   
     
    private Dish dish;
    private int birdId;
    public BabyBird(Dish dish, int birdId){
        this.dish = dish;
        this.birdId = birdId;
    }

    public void run(){
        System.out.println("baby thread strarted");
        while (true) {
            dish.eat(this.birdId);
            try {
                Thread.sleep(ThreadLocalRandom.current().nextInt(2000));
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

}
