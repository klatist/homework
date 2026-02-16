package bees;
import java.util.concurrent.*;
import java.lang.Runnable;

public class Bear implements Runnable{
    private Pot pot;

    public Bear(Pot pot){
        this.pot = pot;
    }

    public void run(){
        System.out.println("bear thread strarted");
        while(true){
            pot.eat();
        }

    }
}