package bees;

import java.util.concurrent.*;
import java.lang.Runnable;

public class Honeybees implements Runnable{
    private Pot pot;
    private int beeId;

    public Honeybees(Pot pot, int beeId){
        this.pot = pot;
        this.beeId = beeId;
    }

    public void run(){
        System.out.println("bee thread strarted");
        while(true){
            pot.fill(beeId);
        }
    }

}