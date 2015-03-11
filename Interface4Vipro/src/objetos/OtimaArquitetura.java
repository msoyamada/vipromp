/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package objetos;

import java.util.Vector;

/**
 *
 * @author pardal
 */
public class OtimaArquitetura {

    private static Vector <inScreen>     arq         = new Vector<inScreen>();
    private static double       potencia    = 0.0;
    private static double       desempenho  = 0.0;
    private static double       fitness     = 999999999;
    private static int          gerac       = 0;
    private static int          ind         = 0;

    public static Vector<inScreen> getArq() {
        return arq;
    }

    public static void setArq(Vector<inScreen> aArq) {
        arq = aArq;
    }

    public static double getPotencia() {
        return potencia;
    }

    public static void setPotencia(double aPotencia) {
        potencia = aPotencia;
    }

    public static double getDesempenho() {
        return desempenho;
    }

    public static void setDesempenho(double aDesempenho) {
        desempenho = aDesempenho;
    }

    public static double getFitness() {
        return fitness;
    }

    public static void setFitness(double aFitness) {
        fitness = aFitness;
    }

    /**
     * @return the gerac
     */
    public static int getGerac() {
        return gerac;
    }

    /**
     * @param aGerac the gerac to set
     */
    public static void setGerac(int aGerac) {
        gerac = aGerac;
    }

    /**
     * @return the ind
     */
    public static int getInd() {
        return ind;
    }

    /**
     * @param aInd the ind to set
     */
    public static void setInd(int aInd) {
        ind = aInd;
    }

}
