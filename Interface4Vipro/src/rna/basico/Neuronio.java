package rna.basico;

import java.text.DecimalFormat;
import rna.basico.funcaoativacao.FuncaoAtivacao;
import rna.basico.funcaoativacao.FuncaoLogistica;

public class Neuronio {

    double[] pesos;
    double bias;
    FuncaoAtivacao funcaoAtivacao;

    //somatório de cada peso multiplicado pela respectiva entrada
    public double combinacaoLinear(double[] entrada) {
        double saida = 0;
        //System.out.println("tam entrada: " + entrada.length + " nro pesos: " + pesos.length);
        if (entrada.length == pesos.length) {            
            for (int i = 0; i < entrada.length; i++) {
               // System.out.println("entrada[i] e peso[i] " + entrada[i] + " "+ pesos[i] );
                saida += entrada[i] * pesos[i];
            }
        }
        //System.out.println("saidaaaaaaaaaaaaaaaa: " + saida);
        return saida;
    }

    //combinacao linear + bias
    public double potencialAtivacao(double[] entrada) {
        double saida = 0;
        if (entrada.length == pesos.length) {
            for (int i = 0; i < entrada.length; i++) {
                saida += entrada[i] * pesos[i];
            }
            saida += bias;
        }
        return saida;
    }

    //aplicacao da funcao de ativacao no potencial de ativacao
    public double sinalFuncional(double[] entrada) {
        double saida = 0;
        if (entrada.length == pesos.length) {
            for (int i = 0; i < entrada.length; i++) {
                saida += entrada[i] * pesos[i];
            }
            saida += bias;
            saida = funcaoAtivacao.calcular(saida);
        }
        return saida;
    }

    public Neuronio() {
        pesos = null;
        bias = 0.0;
        funcaoAtivacao = null;
    }

    public Neuronio(int nroPesos){
        pesos = new double[nroPesos];
//        for (int i = 0; i < nroPesos; i++) {
//            //garantem os pesos iniciais entre 0.0 e 0.5
//            this.pesos[i] = 0.5*Math.random();
//            System.out.println("valor do peso: " + this.pesos[i]);
//        }
        this.bias = 0.5*Math.random();
        //coloca a funcao Logistica como padrao
        this.funcaoAtivacao = new FuncaoLogistica();
        
    }

    public Neuronio(double[] pesos, double bias, FuncaoAtivacao funcaoAtivacao) {
        this.pesos = pesos;
        this.bias = bias;
        this.funcaoAtivacao = funcaoAtivacao;
    }

    public double getBias() {
        return bias;
    }

    public void setBias(double bias) {
        this.bias = bias;
    }

    public FuncaoAtivacao getFuncaoAtivacao() {
        return funcaoAtivacao;
    }

    public void setFuncaoAtivacao(FuncaoAtivacao funcaoAtivacao) {
        this.funcaoAtivacao = funcaoAtivacao;
    }
    
    public double getPeso(int indice){        
        return pesos[indice];
    }
    
    public void setPeso(int indice, double peso){
        //System.out.println(" | peso: " + peso);
        pesos[indice] = peso;
    }

    public int nroPesos(){
        return pesos.length;
    }

//    public double[] getPesos() {
//        return pesos;
//    }

//    public void setPesos(double[] pesos) {
//        this.pesos = pesos;
//    }

    @Override
    public String toString() {
        DecimalFormat df = new DecimalFormat();
        df.setMaximumFractionDigits(10);
        df.setMinimumFractionDigits(10);
        String saida = "";
        saida+=" (*"+df.format(bias);
        for (int i = 0; i < pesos.length; i++) {            
            saida+="\n   "+df.format(pesos[i]);
        }        
        saida+= ")";
        return saida;
    }
}
