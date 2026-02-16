package birds;
import java.util.concurrent.*;

public class Dish{
    private static int W = 10;
    private int worms = W;
    private boolean notifiedPapa;

    public synchronized void eat(int birdId){
        /*if (worms == 0 & notifiedPapa){
            try {
                wait();
            } catch (InterruptedException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }*/

        if(worms == 0 & !notifiedPapa){
            //notify papabird
            System.out.println("CHIRP CHIRP PAPA MORE FOOD");
            notifiedPapa = true;
            this.notifyAll();
        }

        if(worms > 0){
            worms--;
            System.out.println("bird " + birdId + " takes a worm " + worms + " left");
        }

    }

    public synchronized void fill(){
        try {
            while (worms > 0) {
                System.out.println("papa sleeps");
                this.wait();
            }   
            
        } catch (InterruptedException e) {
            System.err.println(e);
        }
        System.out.println("*fills dish* come and eat my children");
        worms = W;
        notifiedPapa = false;
        //this.notifyAll();
        
    }
}


