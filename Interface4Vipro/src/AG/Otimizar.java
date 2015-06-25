// Decompiled by Jad v1.5.8g. Copyright 2001 Pavel Kouznetsov.
// Jad home page: http://www.kpdus.com/jad.html
// Decompiler options: packimports(3)
// Source File Name:   Otimizar.java

package AG;

import java.io.*;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import modelo.Ambiente;
import objetos.OtimaArquitetura;
//import objetos.Warnings;
import objetos.inScreen;
import visao.TelaPrincipal;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Random;
import java.util.Vector;
import java.util.Vector;
import rna.treinamento.BackPropagationMLP;
import rna.redes.MLP;

// Referenced classes of package AG:
//            AGconfig

public class Otimizar
{

    public static String sDir = "";
    public static Vector BD = new Vector();
    public static boolean escreverVet = false;
    public static int     LimiteProcura = 15;
    public static double  PercentPopTreinamento = 0.6;
    public static String aux;

    public static int numNeuroniosEntrada = 46;
    public static String numtipoCamadas = "46";
    public static int numNeuroniosSaida = 1;
    public static String sAvaliacao = "Avaliacao1";

    public static double erroMaxP = 0.0;
    public static double erroMinP = 999999999.99999999999999999999999999999999999;
    public static double somaErrosP = 0.0;

    public static double erroMaxD = 0.0;
    public static double erroMinD = 999999999.99999999999999999999999999999999999;
    public static double somaErrosD = 0.0;

    public static String otimizarFunc(Vector compsNaTela, Vector conexoes){

        String retorno = "ERRO";
        PrintWriter LogOut = null;
        PrintWriter saidaPareto = null;
        PrintWriter showDados = null;

        try{

            Vector arqAux = new Vector();
            Vector indAux = new Vector();
            int numProc;            
            int countConverg;
            Vector populacao;
            Vector Newpopulacao;
            Vector Fitness;
            double desempenho;
            double potencia;
            int gerac;

            int MinArqBaseInicial = 30;
            int MinArqBaseSecundario = 10;
            int NumArqBaseAnt = 0;
            int contNumArqEscre = 0;

            DecimalFormat df = new DecimalFormat();
            df.setMaximumFractionDigits(10);
            df.setMinimumFractionDigits(10);

            //dados da rede neural            
            double taxaAprendizagem = 0.4;
            double momento = 0.6;
            MLP redePotencia = new MLP(numNeuroniosEntrada, numtipoCamadas, numNeuroniosSaida);
            MLP redeDesempen = new MLP(numNeuroniosEntrada, numtipoCamadas, numNeuroniosSaida);
            BackPropagationMLP bcPotencia = new BackPropagationMLP(redePotencia, taxaAprendizagem, momento);
            BackPropagationMLP bcDesempen = new BackPropagationMLP(redeDesempen, taxaAprendizagem, momento);

            //sDir = "/home/pardal/Desktop/AlgoritmosGeneticos/";
            sDir = "/home/vipro/testes/AlgoritmosGeneticos/";

            
            File Log = new File(sDir + "Log");
            LogOut = new PrintWriter(new FileWriter(Log));

            File CurvaPareto = new File (sDir + "Pareto");
            saidaPareto = new PrintWriter(new FileWriter(CurvaPareto));

            File Dados = new File(sDir + "Dados");
            showDados = new PrintWriter(new FileWriter(Dados));

            numProc = ProcCount(compsNaTela);
            
            double erroAceitavelPot = 0.00003;
            double erroAceitavelDes = 0.00003;
            boolean redeTreinada = false;

            boolean trocou = false;
            countConverg = 0;
            populacao = new Vector();
            Newpopulacao = new Vector();
            Fitness = new Vector();
            desempenho = 0.0;
            potencia = 0.0;
            double parcial = 0.0;

            populacao.add((Vector)compsNaTela.clone());

            for(int i = 0; i < AGconfig.getPopIni() - 1; i++)
                populacao.add(criaInd(compsNaTela));

            Ambiente.newDir(sDir);

            for (gerac = 1; gerac <= AGconfig.gerac; gerac++){                

                for(int ind = 0; ind < populacao.size(); ind++){

                    System.out.println("Executando individuo " + (ind + 1) + " da geracao " + gerac);

                    indAux = new Vector((Vector)populacao.get(ind));
                    indAux = new Vector(getFromBD(makeVet(indAux, escreverVet)));

                    if (indAux.size() > 0){

                        potencia = Double.parseDouble(indAux.get(indAux.size() - 2).toString());
                        desempenho = Double.parseDouble(indAux.get(indAux.size() - 1).toString());

                        LogOut.println("Arquitetura já simulada:");
                        LogOut.println("Protencia Media......." + potencia);
                        LogOut.println("Desempenho............" + desempenho);
                        LogOut.println("Fitness..............." + Math.sqrt(Math.pow(potencia,2) + Math.pow(desempenho,2))+ "\n\n\n");
                        System.out.println("individuo: " + (ind + 1) + ", fitness:" + (potencia + desempenho) + "\n");                        
                        
                        Fitness.add(Math.sqrt(Math.pow(potencia,2) + Math.pow(desempenho,2)));
                        LogOut.flush();
                        continue;

                    }

                    Ambiente.setComp(new Vector((Vector)populacao.get(ind)));
                    Ambiente.newDir(sDir + gerac + "_" + ind);
                    Ambiente.Save("");

                    try{

                        File dump = new File(sDir + gerac + "_" + ind + "/Arquitetura");
                        FileWriter writer = new FileWriter(dump);
                        PrintWriter saida = new PrintWriter(writer);
                        saida.print(Ambiente.getDesc());
                        saida.close();
                        writer.close();

                    }catch(Exception e){

                        LogOut.println("ERRO ao criar Arquivo de estrutura");
                        continue;

                    }

                    LogOut.println("Executando individuo " + (ind + 1) + " da geracao " + gerac);

                    if(!redeTreinada || gerac <= 10){

                        LogOut.println("Criando MakeFile:..............." + Ambiente.makeFile(""));
                        LogOut.println("Criando Main:..................." + Ambiente.Main(""));
                        LogOut.println("Criando Scripts:................" + Ambiente.Scripts(""));
                        LogOut.println("Criando Configure:.............." + Ambiente.makeConfigure(""));
                        LogOut.println("Criando Outros:................." + Ambiente.GetFiles(""));
                        LogOut.println("Criando Make e Executando:......");
                        if(Ambiente.CommandMake("").trim().equals("ERRO")){
                            System.out.println("ERRO 01: Erro ao executar indivíduo!");
                            continue;
                        } else
                        if(Ambiente.Executar("").trim().equals("ERRO no executavel")){
                            System.out.println("ERRO 02: Erro ao executar indivíduo!");
                            continue;
                        } else
                            LogOut.println("Arquivo Executado com sucesso");
                    
                        try{

                            File dump = new File(sDir + gerac + "_" + ind + "/dumps");
                            BufferedReader in = new BufferedReader(new FileReader(dump));
                            potencia = 0.0;
                            desempenho = 0.0;

                            try{

                                saidaPareto.println(Ambiente.getDescText());
                                for(int procs = 0; procs < numProc; procs++){

                                    String auxLeu = in.readLine();
                                    String leu[];
                                    for(leu = auxLeu.split(" "); !leu[0].equals("sim_cycle"); leu = auxLeu.split(" "))
                                        auxLeu = in.readLine();

                                    int pos;

                                    for(pos = 1; leu[pos].trim().equals(""); pos++);

                                    // reading performance 
                                    // MSO: do not normalize
                                    //if(Double.parseDouble(leu[pos]) > desempenho)
                                    desempenho = Double.parseDouble(leu[pos]);

                                    auxLeu = in.readLine();
                                    //MSO: changed to cc2
                                    for(leu = auxLeu.split(" "); !auxLeu.split(" ")[0].equals("avg_total_power_cycle_cc2"); leu = auxLeu.split(" "))
                                        auxLeu = in.readLine();

                                    for(pos = 1; leu[pos].trim().equals(""); pos++);

                                    // MSO: do not normalize. 
                                    if(Double.parseDouble(leu[pos])> 0.0){
                                        potencia = Double.parseDouble(leu[pos]) ;
                                    } else {
                                        LogOut.println("ERRO, potencia acima do suportado. Individuo " + ind + " geracao " + gerac + " processador " + procs);
                                        potencia = 999999999 ;
                                    }
                                    saidaPareto.print(":"+desempenho + ":" + potencia);

                                }

                            } catch(Exception e) {
                                retorno = "ERRO ao ler dump";
                            }                            

                            if(potencia <= 0.0 || desempenho <= 0.0){

                                potencia = 999999999;
                                desempenho = 99999999;

                            }
                            //Saida_" + mem + "_" + unFun + "_" + cacheil + "_" + cachedl + "_" + scalar+"_"+ cont +
                            //saidaPareto.println((desempenho * Math.pow(10, 8)) + ":" + (potencia * 100000.00));
                           saidaPareto.println(); 
                            saidaPareto.flush();

                            in.close();
                                                        
                            } catch(IOException e) {
                                LogOut.println("Erro ao Ler: " + gerac + "_" + ind + "\n");
                            }

                            if ((BD.size() == MinArqBaseInicial) || ((BD.size() - NumArqBaseAnt == MinArqBaseSecundario) && NumArqBaseAnt > 0)){

                                String sAuxDados = "";
                                countConverg = 0;

                                System.out.println("Treinando Redes Neurais");
                                contNumArqEscre = 0;
                                for (int treiner = NumArqBaseAnt; treiner < Math.round((NumArqBaseAnt + ((BD.size() - NumArqBaseAnt)) * PercentPopTreinamento)); treiner++){
                                    
                                    double[] arqJaSim = VectorToStatic((Vector) BD.get(treiner));
                                    double[] arrayaux = new double[1];

                                    for (int u = 0; u < arqJaSim.length; u++) sAuxDados += arqJaSim[u] + " ";
                                    showDados.println(sAuxDados);
                                    contNumArqEscre++;
                                    sAuxDados = "";

                                    if (arqJaSim[arqJaSim.length - 2] > 1) arrayaux[0] = 1;
                                    else if(arqJaSim[arqJaSim.length - 2] < -1) arrayaux[0] = -1;
                                    else arrayaux[0] = arqJaSim[arqJaSim.length - 2];
                                    bcPotencia.treinar(VectorToStaticTwoLess((Vector) BD.get(treiner)), arrayaux);

                                    if (arqJaSim[arqJaSim.length - 1] > 1) arrayaux[0] = 1;
                                    else if(arqJaSim[arqJaSim.length - 1] < -1) arrayaux[0] = -1;
                                    else arrayaux[0] = arqJaSim[arqJaSim.length - 1];
                                    bcDesempen.treinar(VectorToStaticTwoLess((Vector) BD.get(treiner)), arrayaux);

                                }
                                System.out.println("Foram usadas " + contNumArqEscre + " arquiteturas para treinamento!");

                                sAuxDados = "";
                                contNumArqEscre = 0;
                                boolean passou = false;
                                System.out.println("Aplicado arquiteturas");
                                for (int tester = (int) Math.round((NumArqBaseAnt + ((BD.size() - NumArqBaseAnt)) * PercentPopTreinamento)); tester < BD.size(); tester++){

                                    double[] arqJaSim = VectorToStatic((Vector) BD.get(tester));
                                    double[] saida1 = redePotencia.calcular(VectorToStaticTwoLess((Vector)BD.get(tester)));
                                    double[] saida2 = redeDesempen.calcular(VectorToStaticTwoLess((Vector)BD.get(tester)));

                                    for (int u = 0; u < arqJaSim.length; u++) sAuxDados += arqJaSim[u] + " ";
                                    showDados.println(sAuxDados);
                                    contNumArqEscre++;
                                    sAuxDados = "";

                                    double erroPotencia = (Math.abs(arqJaSim[arqJaSim.length - 2] - saida1[0]) * (100 / arqJaSim[arqJaSim.length - 2]));
                                    double erroDesempenho = (Math.abs(arqJaSim[arqJaSim.length - 1] - saida2[0]) * (100 / arqJaSim[arqJaSim.length - 1]));

                                    if (erroPotencia > erroMaxP) erroMaxP = erroPotencia;
                                    if (erroPotencia < erroMinP) erroMinP = erroPotencia;
                                    somaErrosP += Math.abs(arqJaSim[arqJaSim.length - 2]);

                                    if (erroDesempenho > erroMaxD) erroMaxD = erroDesempenho;
                                    if (erroDesempenho < erroMinD) erroMinD = erroDesempenho;
                                    somaErrosD += Math.abs(arqJaSim[arqJaSim.length - 1]);

                                    System.out.println("Potencia:   Vipro: " + df.format(arqJaSim[arqJaSim.length - 2]) + "  MLP: " + df.format(saida1[0]) + "  Erro: " + df.format(erroPotencia));
                                    System.out.println("Desempenho: Vipro: " + df.format(arqJaSim[arqJaSim.length - 1]) + "  MLP: " + df.format(saida2[0]) + "  Erro: " + df.format(erroDesempenho));

                                    if ((Math.abs(saida1[0] - arqJaSim[arqJaSim.length - 1]) < erroAceitavelPot) && (Math.abs( arqJaSim[arqJaSim.length - 2] - saida2[0]) < erroAceitavelDes))
                                        passou = true;

                                }
                                System.out.println("Foram usadas " + contNumArqEscre + " arquiteturas para simulação!");
                                System.out.println("============================================================================");

                                if (passou){

                                    redeTreinada = true;
                                    System.out.println("Rede Neural Treinada");

                                }

                                NumArqBaseAnt = BD.size();

                            }

                        } else {

                            indAux = new Vector((Vector)populacao.get(ind));
                            double[] saida1 = redePotencia.calcular(makeStaticVet(indAux, false));
                            double[] saida2 = redeDesempen.calcular(makeStaticVet(indAux, false));

                            potencia = saida1[0];
                            desempenho = saida2[0];

                            LogOut.println("Utilizando Redes Neurais");
                            System.out.println("Utilizando Redes Neurais");

                        }
                        // MSO: lets use the distance to origin (0,0) as the fitness function
                        LogOut.println("Resultados:");
                        LogOut.println("Protencia Media......." + potencia );
                        LogOut.println("Desempenho............" + desempenho);
                        LogOut.println("Fitness..............." + Math.sqrt(Math.pow(potencia,2) + Math.pow(desempenho,2)) + "\n\n\n");
                        System.out.println("individuo: " + (ind + 1) + ", fitness:" + (potencia + desempenho) + "\n");                        
                        
                        Fitness.add(Math.sqrt(Math.pow(potencia,2) + Math.pow(desempenho,2)));

                        indAux = new Vector((Vector)populacao.get(ind));
                        arqAux = makeVet(indAux, false);
                        
                        arqAux.add(potencia);
                        arqAux.add(desempenho);
                        
                        puttyInBD(arqAux);
                        LogOut.flush();

                    
                }

                trocou = false;
                for(int i = 0; i < Fitness.size(); i++){
                    
                    if(Double.parseDouble(Fitness.get(i).toString()) < OtimaArquitetura.getFitness()){

                        OtimaArquitetura.setArq(new Vector((Vector)populacao.get(i)));
                        OtimaArquitetura.setDesempenho(desempenho);
                        OtimaArquitetura.setPotencia(potencia);
                        OtimaArquitetura.setFitness(Double.parseDouble(Fitness.get(i).toString()));
                        OtimaArquitetura.setGerac(gerac);

                        trocou = true;
                        countConverg = 0;
                        //System.out.println("Nova arquitetura encontrada");

                    }

                }

                if(!trocou)
                    countConverg++;

                System.out.println("A melhor arquitetura até o momento foi encontrada " + countConverg + " gerações atras.");

                if(countConverg > LimiteProcura){

                    retorno = ("Arquitetura Convergiu com " + gerac + " geracoes e com um valor fitness de "  + OtimaArquitetura.getFitness());

                    Ambiente.setComp(OtimaArquitetura.getArq());
                    LogOut.println("\n\nA arquitetura convergiu com Sucesso na geracao " + gerac + " com um fitness de " + OtimaArquitetura.getFitness());
                    LogOut.close();

                    return retorno;

                }


                if(gerac < AGconfig.getGerac()){

                    double menorFitness = 0.0;
                    int pos = 0;

                    Vector passaram = new Vector();

                    System.out.println("Passando Indivíduos por Elitismo");
                    for(int elit = 0; elit < AGconfig.getTaxElit(); elit++){

                        menorFitness = 999999999;
                        pos = 0;

                        for(int ind = 0; ind < Fitness.size(); ind++){

                            if(Double.parseDouble(Fitness.get(ind).toString()) < menorFitness && notIn(passaram, pos)){

                                menorFitness = Double.parseDouble(Fitness.get(ind).toString());
                                pos = ind;

                            }

                        }

                        Newpopulacao.add(new Vector((Vector)populacao.get(pos)));
                        passaram.add(new Integer(pos));

                    }

                    System.out.println("Cruzando Genes");
                    while (Newpopulacao.size() < AGconfig.getPopIni()){

                        Vector aux = new Vector(cruzamento(selecao(populacao, Fitness), selecao(populacao, Fitness)));
                        if(aux.size()> 0)
                            Newpopulacao.add(new Vector(aux));

                    }

                    for(int ind = 0; ind < Newpopulacao.size(); ind++){

                        if(Math.random() * 100 < (double)AGconfig.getMut())
                            Newpopulacao.set(ind, mutacao((Vector)Newpopulacao.get(ind)));

                    }

                    System.out.println("Aplicando Mutação\n");
                    populacao = new Vector(Newpopulacao);
                    Newpopulacao = new Vector();
                    Fitness = new Vector();

                }

                LogOut.flush();

            }

            retorno = ("O AG chegou ao seu limite de geracoes. A melhor arquitetura encontrada possui valor fitness de " + OtimaArquitetura.getFitness()).toString();
            Ambiente.setComp(OtimaArquitetura.getArq());

        } catch(IOException e) {
            
            System.out.println("ERRO em algum lugar aki");
            return "ERRO";

        } finally {

            LogOut.println("\n\nMelhor arquitetura foi encontrada na geração: " + OtimaArquitetura.getGerac());
            LogOut.println("Melhor potência: " + (OtimaArquitetura.getPotencia()));
            LogOut.println("Melhor Desempenho: " + (OtimaArquitetura.getDesempenho() ));
            LogOut.close();
            showDados.close();
            saidaPareto.close();

            try{
            
                File Dados = new File(sDir + sAvaliacao);
                showDados = new PrintWriter(new FileWriter(Dados)); 
                
                showDados.println("Configuração da Rede Neural: (" + numNeuroniosEntrada + ", \"" + numtipoCamadas + "\", " + numNeuroniosSaida + ")\n");
                showDados.println("Rede para potencia:");
                showDados.println("Erro máx: " + erroMaxP + "%");
                showDados.println("Erro min: " + erroMinP + "%");
                showDados.println("Erro med: " + (somaErrosP / (AGconfig.getPopIni() * AGconfig.getGerac() * PercentPopTreinamento)));
                showDados.println("\nRede para desempenho:");
                showDados.println("Erro máx: " + erroMaxD + "%");
                showDados.println("Erro min: " + erroMinD + "%");
                showDados.println("Erro med: " + (somaErrosD / (AGconfig.getPopIni() * AGconfig.getGerac() * PercentPopTreinamento)));

                showDados.close();
               
                
            } catch (IOException e) {
                
                return "ERRO";
                
            }



        }

        return retorno;

    }

    private static Vector criaInd(Vector ind){

        Vector newInd = new Vector();
        inScreen proc = new inScreen();

        for(int i = 0; i < ind.size(); i++){

            if(((inScreen)ind.get(i)).getCompType().compareTo("processador") != 0){

                newInd.add(ind.get(i));
                continue;
            }

            proc = new inScreen((inScreen)ind.get(i));
            proc.setCarac(((inScreen)ind.get(i)).getCarac());
            
            //BP
           // proc.getCarac().setElementAt(Ele(5, 7), 13); //#13 BP bimodal table size default 512  range 128 - 4096         
           // proc.getCarac().setElementAt(Ele(5, 7) + " 1" , 17); //#17 BTB config default 512 1   range 128 - 4096

            //Scalarity
           // proc.getCarac().setElementAt(Ele(5, 1), 19);  // #19 IF queue size 2 to 64
            //MSO: scalarity is the same for ID, II, IC range 1 to 8
            long scalarity= Ele(3, 0); 
            proc.getCarac().setElementAt(scalarity, 21);  // #21 ID size 4
            proc.getCarac().setElementAt(scalarity, 22);  // #22 II size 4

            if(Ele(10, 0)> 4L)
                proc.getCarac().setElementAt("true", 23);  // #23 In-order
            else
                proc.getCarac().setElementAt("false", 23); // #23 Out-of-order

            proc.getCarac().setElementAt(scalarity, 25);  // #25 IC size 4
            proc.getCarac().setElementAt(Ele(5, 2), 26);  // #26 RUU size 16 range 4 to 128
            proc.getCarac().setElementAt(Ele(5, 2), 27);  // #27 LS size 16 range 4 to 128
            
            //Functional units
            proc.getCarac().setElementAt(Uni(7, 1), 28); // #28 ALU  1  to 8
           // proc.getCarac().setElementAt(Uni(7, 1), 29); // #29 MULT/DIV 1 to 8
           // proc.getCarac().setElementAt(Uni(1, 1), 30); // #30 MEM ports 2 to 8
           // proc.getCarac().setElementAt(Uni(7, 1), 31); // #31 FP 1 to 8
           // proc.getCarac().setElementAt(Uni(7, 1), 32); // #32 FP Mult 1 to 8

            
            // cache
            int par1 = (int) Ele(13, 3); // #blocks range 8 to 64K
            //int par2 = (int) Ele(3, 3);  // # block size 8 to 64
            int par2 = 32;  // # block size fixed to 32
            proc.getCarac().setElementAt("il1:" + par1 + ":" + par2 + ":1:l", 33); //# il1 config
            /*while ((par1 * par2) > 4194304){  

                par1 = (int) Ele(13, 3);
                par2 = (int) Ele(3, 3);

                proc.getCarac().setElementAt("il1:" + par1 + ":" + par2 + ":1:l", 33);

            }*/
            // cache associativity
            int dl;
            /*dl = (int) Math.round(Math.random());
            if (dl == 0) dl = 1;
            else dl = 4;*/
            dl= 4; // MSO fixed to 4
             
            par1 = (int) Ele(13, 3); // #blocks
            //par2 = (int) Ele(3, 3);  // # block size
            par2 = 32;  // # block size fixed to 32
            
            proc.getCarac().setElementAt("dl1:" + par1 + ":" + par2 + ":" + dl + ":l", 35);
            
     /*       while ((par1 * par2 * dl) > 4194304){
            
                dl = (int) Math.round(Math.random());            
                if (dl == 0) dl = 1;
                else dl = 4;            
                
                par1 = (int) Ele(13, 3);
                par2 = (int) Ele(3, 3);
            
                proc.getCarac().setElementAt("dl1:" + par1 + ":" + par2 + ":" + dl + ":l", 35);
                
            }*/


           // memort latency MSO fixed
            proc.getCarac().setElementAt("18 2", 44); // latency 18 2
            proc.getCarac().setElementAt("8", 45); // bus width
            proc.getCarac().setElementAt("itlb1:4096:4096:4:l", 46); // itlb itlb1:4096:4096:4:l
            proc.getCarac().setElementAt("dtlb1:4096:4096:4:l", 47); // dtlb dtlb1:4096:4096:4:l
            proc.getCarac().setElementAt("70", 48); // #48 memory latency 70
            newInd.add(proc);

        }

        return newInd;
    }

    public static long Ele(int ele, int start){
        return (long)Math.pow(2, (long)start + Math.round(Math.random() * (double)ele));
    }

    public static long Uni(int ele, int start){
        return (long)start + Math.round(Math.random() * (double)ele);
    }

    public static long Duni(int ele, int start){
        return (long)start + 2L * Math.round(Math.random() * (double)(ele / 2));
    }

    public static int ProcCount(Vector Arq){

        int count = 0;
        for(int i = 0; i < Arq.size(); i++){

            if(((inScreen)Arq.get(i)).getCompType().equals("processador"))
                count++;

        }

        return count;

    }

    public static Vector cruzamento(Vector pai, Vector mae){

        Vector filho = new Vector(pai);

        if(Math.random() * 100D < (double)AGconfig.getCruz()){

            for(int i = pai.size() / 2; i < mae.size(); i++)
                filho.set(i, mae.get(i));

            return filho;

        } else {

            return new Vector(0);

        }
    }

    public static Vector selecao(Vector populacao, Vector fitness){

        if(AGconfig.getSelec().equals("Método do Torneio")){

            Vector vencedores = new Vector(fitness.size());

            for(int i = 0; i < fitness.size(); i++)
                vencedores.add(new Integer(0));

            int ind1 = 0;
            int ind2 = 0;
            int ind3 = 0;

            for(int count = 0; count < AGconfig.getPopIni() / 2; count++){

                ind1 = (int)(Math.random() * (double)(fitness.size() - 1));
                ind2 = (int)(Math.random() * (double)(fitness.size() - 1));
                ind3 = (int)(Math.random() * (double)(fitness.size() - 1));

                if(Double.parseDouble(fitness.get(ind1).toString()) < Double.parseDouble(fitness.get(ind2).toString()) && Double.parseDouble(fitness.get(ind1).toString()) < Double.parseDouble(fitness.get(ind3).toString()))
                {
                    vencedores.set(ind1, Integer.valueOf(((Integer)vencedores.get(ind1)).intValue() + 1));
                    continue;
                }
                if(Double.parseDouble(fitness.get(ind2).toString()) < Double.parseDouble(fitness.get(ind3).toString()))
                    vencedores.set(ind2, Integer.valueOf(((Integer)vencedores.get(ind2)).intValue() + 1));
                else
                    vencedores.set(ind3, Integer.valueOf(((Integer)vencedores.get(ind3)).intValue() + 1));
            }

            int pos = 0;
            int max = 999999999;

            for(int i = 0; i < vencedores.size(); i++){
                if(((Integer)vencedores.get(i)).intValue() < max){

                    max = ((Integer)vencedores.get(i)).intValue();
                    pos = i;

                }
            }

            return new Vector((Vector)populacao.get(pos));

        } else {
            return new Vector();
        }
    }

    public static Vector mutacao(Vector ind){

        Vector newInd = new Vector();
        inScreen proc = new inScreen();
        int Taxa = AGconfig.getMut();

        for(int i = 0; i < ind.size(); i++){

            if(((inScreen)ind.get(i)).getCompType().compareTo("processador") != 0){

                newInd.add(ind.get(i));
                continue;

            }

            proc = new inScreen((inScreen)ind.get(i));
            proc.setCarac(((inScreen)ind.get(i)).getCarac());

            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(7, 5)), 13);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt((new StringBuilder()).append(Ele(7, 5)).append(" ").append(Ele(2, 0)).toString(), 17);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(6, 0)), 19);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(6, 0)), 21);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(6, 0)), 22);
            if(Math.random() * 100D < (double)Taxa)
                if(Ele(10, 0)> 4L)
                    proc.getCarac().setElementAt("true", 23);
                else
                    proc.getCarac().setElementAt("false", 23);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(6, 0)), 25);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(5, 3)), 26);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Ele(5, 3)), 27);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Uni(7, 1)), 28);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Uni(7, 1)), 29);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Uni(1, 1)), 30);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Uni(7, 1)), 31);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Uni(7, 1)), 32);

            int par1 = 0;
            int par2 = 0;

            if(Math.random() * 100D < (double)Taxa){
                
                par1 = (int) Ele(13, 3);
                par2 = (int) Ele(3, 3);
            
                proc.getCarac().setElementAt("il1:" + par1 + ":" + par2 + ":1:l", 33);            
                while ((par1 * par2) > 4194304){
            
                    par1 = (int) Ele(13, 3);
                    par2 = (int) Ele(3, 3);
            
                    proc.getCarac().setElementAt("il1:" + par1 + ":" + par2 + ":1:l", 33);
                
                }
                
            }




            if(Math.random() * 100D < (double)Taxa){
             
                int dl = (int) Math.round(Math.random());            
                if (dl == 0) dl = 1;
                else dl = 4;            

                par1 = (int) Ele(13, 3);
                par2 = (int) Ele(3, 3);

                proc.getCarac().setElementAt("dl1:" + par1 + ":" + par2 + ":" + dl + ":l", 35);

                while ((par1 * par2 * dl) > 4194304){

                    dl = (int) Math.round(Math.random());            
                    if (dl == 0) dl = 1;
                    else dl = 4;            

                    par1 = (int) Ele(13, 3);
                    par2 = (int) Ele(3, 3);

                    proc.getCarac().setElementAt("dl1:" + par1 + ":" + par2 + ":" + dl + ":l", 35);

                }
                
            }


            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt((new StringBuilder()).append(Duni(10, 8)).append(" 2").toString(), 44);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt((new StringBuilder()).append("itlb1:").append(Ele(6, 8)).append(":4096:4:l").toString(), 46);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt((new StringBuilder()).append("dtlb1:").append(Ele(6, 8)).append(":4096:4:l").toString(), 47);
            if(Math.random() * 100D < (double)Taxa)
                proc.getCarac().setElementAt(Long.valueOf(Uni(50, 50)), 48);
            newInd.add(proc);
        }

        return newInd;
    }

    public static boolean notIn(Vector passaram, int pos){

        for(int i = 0; i < passaram.size(); i++)
            if(((Integer)passaram.get(i)).intValue() == pos)
                return false;

        return true;
    }

    public static boolean puttyInBD(Vector arq){

        BD.add(arq);
        return true;

    }

    public static Vector getFromBD(Vector arq){

        Vector arqInBD = new Vector();
        boolean igual = true;

        for(int i = 0; i < BD.size(); i++){

            igual = true;
            arqInBD = (Vector) BD.get(i);

            for (int z = 0; z < arqInBD.size() - 2; z++)
                if (!(arqInBD.get(z).equals(arq.get(z)))) igual = false;

            if (igual) return arqInBD;            

        }

        return new Vector();

    }

    public static Vector makeVet(Vector aux, boolean escreve){

        Vector saida = new Vector();
        inScreen comp = new inScreen();

        for(int i = 0; i < aux.size(); i++){

            comp = (inScreen)aux.get(i);
            if(!comp.getCompType().equals("processador"))
                continue;

            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(13).toString()) / Math.pow(2, 12)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(17).toString().split(" ")[0]) / Math.pow(2, 12)));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(17).toString().split(" ")[1]) / 4));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(19).toString()) / Math.pow(2, 6)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(21).toString()) / Math.pow(2, 6)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(22).toString()) / Math.pow(2, 6)));

            if(comp.getCarac().get(23).toString().equals("false"))
                saida.add(Integer.valueOf(0));
            else
                saida.add(Integer.valueOf(1));

            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(25).toString()) / Math.pow(2, 6)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(26).toString()) / Math.pow(2, 8)));
            
            //Pos > 1
            saida.add(Double.valueOf((double) Integer.parseInt(comp.getCarac().get(27).toString()) / Math.pow(2, 8)));

            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(28).toString()) / 8));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(29).toString()) / 8));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(30).toString()) / 2));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(31).toString()) / 8));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(32).toString()) / 8));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(33).toString().split(":")[1]) / Math.pow(2, 20)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(33).toString().split(":")[2]) / Math.pow(2, 6)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(35).toString().split(":")[1]) / Math.pow(2, 20)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(35).toString().split(":")[2]) / Math.pow(2, 6)));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(44).toString().split(" ")[0]) / 18));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(46).toString().split(":")[1]) / Math.pow(2, 14)));
            saida.add(Double.valueOf((double)Integer.parseInt(comp.getCarac().get(47).toString().split(":")[1]) / Math.pow(2, 14)));
            saida.add(Integer.valueOf(Integer.parseInt(comp.getCarac().get(48).toString()) / 100));
        }

        if (escreve){
            System.out.println("\n\nVETOR NORMALIZADO");
            for (int i = 0; i < saida.size(); i++) System.out.println(saida.get(i));
            System.out.println("\n\n");
        }

        for (int i = 0; i < saida.size(); i++){

            if (Double.parseDouble(saida.get(i).toString()) > 1) System.out.println("ERRO2 MakeVet!! Posição " + i + " maior que 1");

        }
        
        return CorrecaoValores(saida);
    }


    public static double[] makeStaticVet(Vector aux, boolean escreve){

        double[] saida = new double[46];
        int count = 0;
        inScreen comp = new inScreen();

        for(int i = 0; i < aux.size(); i++){

            comp = (inScreen)aux.get(i);
            if(!comp.getCompType().equals("processador"))
                continue;

            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(13).toString()) / Math.pow(2, 12)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(17).toString().split(" ")[0]) / Math.pow(2, 12)));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(17).toString().split(" ")[1]) / 4));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(19).toString()) / Math.pow(2, 6)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(21).toString()) / Math.pow(2, 6)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(22).toString()) / Math.pow(2, 6)));

            if(comp.getCarac().get(23).toString().equals("false"))
                saida[count++] = (Integer.valueOf(0));
            else
                saida[count++] = (Integer.valueOf(1));

            /*System.out.println("\n\n");
            for (int s = 0; s < comp.getCarac().size(); s++)
                System.out.println(s + " " + comp.getCarac().get(s));

            System.out.println("\n\n");*/

            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(25).toString()) / Math.pow(2, 6)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(26).toString()) / Math.pow(2, 8)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(27).toString()) / Math.pow(2, 6)));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(28).toString()) / 8));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(29).toString()) / 8));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(30).toString()) / 2));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(31).toString()) / 8));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(32).toString()) / 8));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(33).toString().split(":")[1]) / Math.pow(2, 20)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(33).toString().split(":")[2]) / Math.pow(2, 6)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(35).toString().split(":")[1]) / Math.pow(2, 20)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(35).toString().split(":")[2]) / Math.pow(2, 6)));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(44).toString().split(" ")[0]) / 18));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(46).toString().split(":")[1]) / Math.pow(2, 14)));
            saida[count++] = (Double.valueOf((double)Integer.parseInt(comp.getCarac().get(47).toString().split(":")[1]) / Math.pow(2, 14)));
            saida[count++] = (Integer.valueOf(Integer.parseInt(comp.getCarac().get(48).toString()) / 100));

        }

        if (escreve){

            System.out.println("\n\nVETOR ESTATICO NORMALIZADO");
            for (int i = 0; i < saida.length - 1; i++) System.out.println(saida[i]);

        }

        for (int i = 0; i < saida.length; i++){

            if (saida[i] > 1) System.out.println("ERRO2 MakeStaticVet!! Posição " + i + " maior que 1");

        }

        return saida;

    }

    public static double[] VectorToStatic(Vector entrada){
        
        double[] saida = new double[entrada.size()];

        for (int i = 0; i < saida.length; i++){

            saida[i] = Double.parseDouble(entrada.get(i).toString());
            if (saida[i] > 1) System.out.println("ERRO2 VectorToStatic!! Posição " + i + " maior que 1");

        }        

        return saida;

    }

     public static double[] VectorToStaticTwoLess(Vector entrada){

        double[] saida = new double[entrada.size() - 2];

        for (int i = 0; i < saida.length - 3; i++){

            saida[i] = Double.parseDouble(entrada.get(i).toString());
            aux += saida[i] + " ";
            if (saida[i] > 1) System.out.println("ERRO2 VectorToStaticTwoLess!! Posição " + i + " maior que 1");

        }

        aux += "\n";

        return saida;

    }

     public static Vector CorrecaoValores(Vector entrada){

         for (int i = 0; i < entrada.size(); i++){

             entrada.set(i, (Double.parseDouble(entrada.get(i).toString()) * 2.0) - 1);

         }

         return entrada;

     }

}