package birds;
import java.util.concurrent.*;
import java.lang.Runnable;

public class PapaBird implements Runnable {
    private Dish dish;

    public PapaBird(Dish dish){
        this.dish = dish;
    }

    public void run(){
        System.out.println("papa thread strarted");
        while(true){
            dish.fill();
        }
    }
}
