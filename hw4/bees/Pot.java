package bees;
import java.util.concurrent.*;

public class Pot{
    private static int H = 10;
    private int honey = 0;
    private boolean notifiedBear;

    public synchronized void fill(int beeId){
    
        if(this.honey < H){
            System.out.println("bee "+ beeId + " adds honey to the pot");
            this.honey++;
        }

        if(this.honey >= H && !notifiedBear){
            this.notifyAll();
            notifiedBear = true;
        }
    }

    public synchronized void eat(){
        try {
            while (this.honey < H) {
                wait();
            }
        } catch (InterruptedException e) {
            // TODO: handle exception
        }
        this.honey = 0;
        System.out.println(this.honey +" honey in the pot *bear eats all honey* NAMNAMNAM");
        notifiedBear = false;
       
    }

}
