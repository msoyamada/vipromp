/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package modelo;

import AG.Otimizar;
import java.awt.Point;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import objetos.OtimaArquitetura;
import objetos.inScreen;


/**
 *
 * @author alunos
 */
public class Ambiente {

    public static Vector<inScreen> compsNaTela = new Vector<inScreen>(); //container de componentes na tela
    public static Vector<String> conexoes = new Vector<String>(); //componentes

    public static boolean haveSelect = false;

    public static String conect1 = "vazio";
    public static String comandoExec = "";

    public static String dir = "/home/vipro/testes";
    // Array to store the MinMax for variar method
    public static int[][] minmaxvariar=new int[5][2];

    public static void Simula(String Comando){

        

    }

    public static String makeFile(String Comando){

        File makeFile = new File(dir + "Makefile");
        String retorno = "ERRO";

        FileWriter writer = null;
        PrintWriter saida = null;

        try {

            writer = new FileWriter(makeFile);
            saida = new PrintWriter(writer);

            saida.println("");
            saida.println("CXX = g++");
            saida.println("CC  = gcc");
            saida.println("AFLAGS = -Wall -w -g");
            saida.println("#CFLAGS =  ");
            saida.println("VIPRO_DIR= /home/vipro/vipromp/VIPRO-MPv0.2/VIPRO-MP/");
            saida.println("LFLAGS =   -L$(VIPRO_DIR) -L$(VIPRO_DIR)systemc-2.3.1/lib-linux -Wall -w -g -lm -Xlinker -zmuldefs -lpthread");
            saida.println("CXXFLAGS=-g -I$(VIPRO_DIR) -I$(VIPRO_DIR)systemc-2.3.1/include -DSYSTEMC -fpermissive");
            saida.println("MFLAGS = `./sysprobe -flags`");
            saida.println("MLIBS  = `./sysprobe -libs` -lm");
            saida.println("ENDIAN = `./sysprobe -s`");
            saida.println("MAKE = make");
            saida.println("AR = ar qcv");
            saida.println("AROPT =");
            saida.println("RANLIB = ranlib");
            saida.println("RM = rm -f");
            saida.println("RMDIR = rm -r -f");
            saida.println("LN = ln -s");
            saida.println("LNDIR = ln -s");
            saida.println("DIFF = diff");
            saida.println("OEXT = o");
            saida.println("LEXT = a");
            saida.println("EEXT =");
            saida.println("CS = ;");
            saida.println("X=/");

            saida.println("");
            saida.println("");

            saida.println("COMPONENTS = $(VIPRO_DIR)bus/libbus.$(LEXT) $(VIPRO_DIR)SSpp-mips/libmips.$(LEXT)");
            //saida.println("OBJS =	main_sc.o timer.o sharedmemory.o");
            saida.println("OBJS =	main_sc.o $(VIPRO_DIR)sharedmemory.o");

            saida.println("");

            saida.println("all:	vipro-mp");

            saida.println("");

            saida.println("vipro-mp:	$(OBJS) $(COMPONENTS) $(VIPRO_DIR)cacti/libcacti.a");
            saida.println("	$(RM) vipro-mp");
            saida.println("	$(CXX) $(LFLAGS) -o vipro-mp $(OBJS) $(COMPONENTS) $(VIPRO_DIR)cacti/libcacti.a $(VIPRO_DIR)/systemc-2.3.1/lib-linux/libsystemc.a -lpthread");

            saida.println("");

            saida.println("#cacti/libcacti.a: ");
            saida.println("#		cd cacti $(CS) \\");
            saida.println("#		$(MAKE) \"MAKE=$(MAKE)\" \"CC=$(CC)\" \"AR=$(AR)\" \"AROPT=$(AROPT)\" \"RANLIB=$(RANLIB)\" \"CFLAGS=$(FFLAGS) $(SAFEOFLAGS)\" \"OEXT=$(OEXT)\" \"LEXT=$(LEXT)\" \"EEXT=$(EEXT)\" \"X=$(X)\" \"RM=$(RM)\" libcacti.$(LEXT)");

            saida.println("");

            saida.println(".cxx.o:	$*.cpp");
            saida.println("	$(CXX) $(CFLAGS) -c $*.cpp");

            saida.println("");

            saida.println("clean:");
            saida.println("	rm -f *.o vipro-mp");

            saida.println("");
            

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            return retorno;

        } finally {
            
            try {
                writer.close();
                saida.close();
            } catch (IOException ex) {
                Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            }

        }

        retorno = "Sucesso";
        return retorno;

    }

    public static String Scripts(String Comando){

        File makeFile = new File(dir + "Script");
        String retorno = "ERRO";

        FileWriter writer = null;
        PrintWriter saida = null;

        try {

            writer = new FileWriter(makeFile);
            saida = new PrintWriter(writer);
            saida.println("export LD_LIBRARY_PATH=/home/vipro/Downloads/systemc-2.3.1/lib-linux:$LD_LIBRARY_PATH");
            int n_procs = 0;
            for (int i = 0; i < compsNaTela.size(); i++) if (compsNaTela.get(i).getCompType().equals("processador")) n_procs++;

            saida.println("./vipro-mp -config -dct int -outfile 2 \\");
            comandoExec = "./vipro-mp -config  \\";

            for (int i = 0; i < compsNaTela.size(); i++) if (compsNaTela.get(i).getCompType().equals("processador")){

                
                saida.println(compsNaTela.get(i).getCarac().get(49) + " \\");
                comandoExec += "\n" + compsNaTela.get(i).getCarac().get(49) + " \\";
                

            }            

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            return retorno;

        } finally {

            try {
                writer.close();
                saida.close();
            } catch (IOException ex) {
                Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            }

        }

        retorno = "Sucesso";
        return retorno;

    }

    public static String Main(String Comando){

        File makeFile = new File(dir + "main_sc.cpp");
        String retorno = "ERRO";

        FileWriter writer = null;
        PrintWriter saida = null;

        try {

            writer = new FileWriter(makeFile);
            saida = new PrintWriter(writer);

            saida.println("#include <systemc.h>");
            saida.println("#include <unistd.h>");
            saida.println("");
            saida.println("/*MSG*/");
            saida.println("#include \"SSpp-mips/simplescalar.hpp\"");
	    saida.println("#include \"bus/my_bus.hpp\"");
	    saida.println("#include \"bus/simple_bus_arbiter.hpp\"");
	    saida.println("#include \"timer.hpp\"");
	    saida.println("#include \"sharedmemory.hpp\"");
            saida.println("");
            saida.println("/*MSO (06/06/2008) Variable declared to obtain the environ directly");
            saida.println("  In SystemC programs we dont have the main, but sc_main*/");
            saida.println("extern char **environ;");
            saida.println("");            
            saida.println("int sc_main(int argc, char *argv[]){");

	    saida.println("");
	    saida.println("");

	    inScreen aux = new inScreen();
            for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("memoria")) saida.println("  sharedmemory 	  *" + aux.getCompName() + "  = new sharedmemory(\"" + aux.getCompName() + "\",/*base*/ " + aux.getCarac().get(0) + ",/*final*/ " + aux.getCarac().get(1) + ",/*latency*/ " + aux.getCarac().get(2) + ");");

	    }

	    for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("barramento")){

		    saida.println("  simple_bus_arbiter *arbiter_" + aux.getCompName() + " = new simple_bus_arbiter(\"arbiter_" + aux.getCompName() + "\");");
		    saida.println("  my_bus 			 *" + aux.getCompName() + " 	  = new my_bus(\"" + aux.getCompName() + "\", /*mode verbose*/ false);");

		}

	    }

	    saida.println("//  timer 	*int_generator    = new timer(\"int_generator\", /*number of interrupt*/ 1);");

	    saida.println("");

	    saida.println("	/* atribuindo os paramentro dos processadores */");
	    saida.println("	//-----------------------------------------------------------------------------------");
            saida.println("");

	    int cont = 1;

            int n_procs = 0;
            int contador = 6;            
            for (int i = 0; i < compsNaTela.size(); i++) if (compsNaTela.get(i).getCompType().equals("processador")) n_procs++;

	    for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("processador")){
                   //MSO I will need the rewrite this function
                 /*   if (cont == 1){
                        saida.println("     const int argc1 =4;");                                	
                        saida.println("     char *argv1[argc1] = {argv[0],argv[1],argv[2], argv[3]};");
                    }else {
                        // PROC2
                        saida.println("     const int argc2 = 4;");
                        saida.println("     char *argv2[argc2] = {argv[0],argv[1],argv[4],argv[5]};");
                    }*/
                    
                    
		   if (cont == 1){

			saida.println("	    const int argc" + cont + " = 10;");
			saida.println("	    char *argv1[argc1] = {argv[0],argv[1],argv[6],argv[7], argv[2], argv[3],");
			saida.println("			  argv[4],argv[8],argv[9], argv[5]};");
			
		    }else{

			saida.println("	    const int argc" + cont + " = 9;");
			saida.println("	    char *argv" + cont + "[argc" + cont + "] = {argv[0],argv[1],argv[10],argv[11], argv[2], argv[3],");
			saida.println("			  argv[4],argv[12],argv[13]};");

		    }

                    saida.println("");
	            cont++;

		}

	    }
	    
	    saida.println("	//-----------------------------------------------------------------------------------");
	    saida.println("");

            saida.println("	FILE *fd= fopen(\"" + dir + "dumps\", \"w+\" );");
            saida.println("	sc_signal<bool> reset;");
            saida.println("");
            saida.println("	/* instanciando o clock da simulação */");
            saida.println("	sc_clock clock(\"CLOCK\", 10, 0.5, 0.0);");
            saida.println("");
            saida.println("	/* ligando o sinal de clock na porta de clock dos componentes */");

	    for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("barramento")) saida.println("	" + aux.getCompName() +"->clock(clock);");
		if (aux.getCompType().equals("memoria")) saida.println("	" + aux.getCompName() +"->clock(clock);");

	    }

            saida.println("//	int_generator->clock(clock);");
            saida.println("");

	    for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("memoria")) saida.println("	" + getBarrIn(aux.getCompName()) + "->slave_port(*" + aux.getCompName() + ");");

	    }

	    for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("barramento")) saida.println("	" + aux.getCompName() + "->arbiter_port(*arbiter_" + aux.getCompName() + ");");

	    }

            saida.println("");

            saida.println("	/* Processador */");

	    cont = 1;
	    for (int i = 0; i < compsNaTela.size(); i++){

		aux = compsNaTela.get(i);
		if (aux.getCompType().equals("processador")){

		    saida.println("	simplescalar scsp" + cont + "(\"" + aux.getCompName() + "\", " + cont + ");");
		    saida.println("	scsp" + cont + ".setFd(fd);");
		    saida.println("	scsp" + cont + ".CLK(clock);");
		    saida.println("	scsp" + cont + ".bus_port(*" + getBarrIn(aux.getCompName()) + ");");
		    saida.println("//	int_generator->interrupt_port(scsp" + cont + ");");
		    saida.println("	scsp" + cont + ".init(argc" + cont + ", argv" + cont + ", environ);");
		    saida.println("	scsp" + cont + ".reset(reset);");
		    saida.println("");

		    cont++;

		}

	    }

            saida.println("	cout << \"Starting\";");
            saida.println("	sc_start(4e9, SC_NS);							// iniciando o clock e todas as threads");
            saida.println("	cout << \"Finished SystemC simulation\" << endl;");
            saida.println("	sc_stop();");
            saida.println("");
            saida.println("	//mshared->print_mem(0x80090000, 0x8fffffff);	// imprime a memoria compartilhada, dado o intervalo");
            saida.println("");
            saida.println("	return 0;");
            saida.println("}");

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            return retorno;

        } finally {
                        
            try {
                writer.close();
                saida.close();
            } catch (IOException ex) {
                Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            }
            
        }


        retorno = "Sucesso";
        return retorno;

    }
    
    public static String makeConfigure(String Comando){
        
        for (int i = 0; i < compsNaTela.size(); i++) if (compsNaTela.get(i).getCompType().equals("processador")) Configure(i);
        
        return "Sucesso";
        
    }

    public static String Configure(int Pos){

        File configure = new File(dir + "configure_" + compsNaTela.get(Pos).getCompName());
        String retorno = "ERRO";

        Vector caracs = compsNaTela.get(Pos).getCarac();

         FileWriter writer = null;
         PrintWriter saida = null;

        try {

            writer = new FileWriter(configure);
            saida = new PrintWriter(writer);
            int i = 0;

            saida.println("##############################################################################");
            saida.println("##  GERAL");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("# random number generator seed (0 for timer seed)");
            saida.println("-seed                             " + caracs.get(i++));
            saida.println("");
            saida.println("# initialize and terminate immediately");
            saida.println("# -q                          " + caracs.get(i++));
            saida.println("");
            saida.println("# restore EIO trace execution from <fname>");
            saida.println("# -chkpt                     " + caracs.get(i++));
            saida.println("");
            saida.println("# redirect simulator output to file (non-interactive only)");
            saida.println("# -redir:sim                 " + caracs.get(i++));
            saida.println("");
            saida.println("# redirect simulated program output to file");
            saida.println("# -redir:prog                " + caracs.get(i++));
            saida.println("");
            saida.println("# simulator scheduling priority");
            saida.println("-nice                             " + caracs.get(i++));
            saida.println("");
            saida.println("# maximum number of inst's to execute");
            saida.println("-max:inst                         " + caracs.get(i++));
            saida.println("");
            saida.println("# number of insts skipped before timing starts");
            saida.println("-fastfwd                          " + caracs.get(i++));
            saida.println("");
            saida.println("# generate pipetrace, i.e., <fname|stdout|stderr> <range>");
            saida.println("# -ptrace                    " + caracs.get(i++));
            saida.println("");
            saida.println("# profile stat(s) against text addr's (mult uses ok)");
            saida.println("# -pcstat                    " + caracs.get(i++));
            saida.println("");
            saida.println("# operate in backward-compatible bugs mode (for testing only)");
            saida.println("-bugcompat                    " + caracs.get(i++));

            saida.println("");
            saida.println("");

            saida.println("##############################################################################");
            saida.println("##  BRANCH PREDICTOR");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("# extra branch mis-prediction latency");
            saida.println("-fetch:mplat                      " + caracs.get(i++));
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# branch predictor type {nottaken|taken|perfect|bimod|2lev|comb}");
            saida.println("-bpred                        " + caracs.get(i++));
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# bimodal predictor config (<table size>)");
            saida.println("-bpred:bimod           " + caracs.get(i++));
            saida.println("");
            saida.println("# 2-level predictor config (<l1size> <l2size> <hist_size> <xor>)");
            saida.println("-bpred:2lev            " + caracs.get(i++));
            saida.println("");
            saida.println("# combining predictor config (<meta_table_size>)");
            saida.println("-bpred:comb            " + caracs.get(i++));//15
            saida.println("");
            saida.println("# return address stack size (0 for no return stack)");
            saida.println("-bpred:ras                        " + caracs.get(i++));//16
            saida.println("");
            saida.println("# BTB config (<num_sets> <associativity>)");
            saida.println("-bpred:btb             " + caracs.get(i++));//17

            saida.println("");
            saida.println("");

            saida.println("##############################################################################");
            saida.println("##  CARACTERISTICAS GERAIS - SUPERESCALAR");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("# speed of front-end of machine relative to execution core");
            saida.println("-fetch:speed                      " + caracs.get(i++));//18
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# instruction fetch queue size (in insts)");
            saida.println("-fetch:ifqsize                    " + caracs.get(i++));
            saida.println("");
            saida.println("# speculative predictors update in {ID|WB} (default non-spec)");
            saida.println("# -bpred:spec_update         " + caracs.get(i++));
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# instruction decode B/W (insts/cycle)");
            saida.println("-decode:width                     " + caracs.get(i++));
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# instruction issue B/W (insts/cycle)");
            saida.println("-issue:width                      " + caracs.get(i++));
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# run pipeline with in-order issue");
            saida.println("-issue:inorder                " + caracs.get(i++));//23
            saida.println("");
            saida.println("# issue instructions down wrong execution paths");
            saida.println("-issue:wrongpath               " + caracs.get(i++));
            saida.println("");
            saida.println("# instruction commit B/W (insts/cycle)");
            saida.println("-commit:width                     " + caracs.get(i++));
            saida.println("");
            saida.println("# register update unit (RUU) size");
            saida.println("-ruu:size                        " + caracs.get(i++));
            saida.println("");
            saida.println("# load/store queue (LSQ) size");
            saida.println("-lsq:size                        " + caracs.get(i++));//27

            saida.println("");
            saida.println("");

            saida.println("##############################################################################");
            saida.println("##  UNIDADES FUNCIONAIS");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("# total number of integer ALU's available");
            saida.println("-res:ialu                         " + caracs.get(i++));//28
            saida.println("");
            saida.println("# total number of integer multiplier/dividers available");
            saida.println("-res:imult                        " + caracs.get(i++));//29
            saida.println("");
            saida.println("# total number of memory system ports available (to CPU)");
            saida.println("-res:memport                      " + caracs.get(i++));//30
            saida.println("");
            saida.println("# total number of floating point ALU's available");
            saida.println("-res:fpalu                        " + caracs.get(i++));//31
            saida.println("");
            saida.println("# total number of floating point multiplier/dividers available");
            saida.println("-res:fpmult                       " + caracs.get(i++));//32

            saida.println("");
            saida.println("");

            saida.println("##############################################################################");
            saida.println("##  CACHE");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("############## Level 1");
            saida.println("");
            saida.println("# l1 inst cache config, i.e., {<config>|dl1|dl2|none}");
            saida.println("-cache:il1             " + caracs.get(i++));//33
            saida.println("");
            saida.println("# l1 instruction cache hit latency (in cycles)");
            saida.println("-cache:il1lat                     " + caracs.get(i++));//34
            saida.println("");
            saida.println("# l1 data cache config, i.e., {<config>|none}");
            saida.println("-cache:dl1             " + caracs.get(i++));//35
            saida.println("");
            saida.println("# l1 data cache hit latency (in cycles)");
            saida.println("-cache:dl1lat                     " + caracs.get(i++));//36
            saida.println("");
            saida.println("############## Level 2");
            saida.println("");
            saida.println("# l2 data cache config, i.e., {<config>|none}");
            saida.println("-cache:dl2             " + caracs.get(i++));//37
            saida.println("");
            saida.println("# <MSG>");
            saida.println("# l2 data cache hit latency (in cycles)");
            saida.println("-cache:dl2lat                     " + caracs.get(i++));//38
            saida.println("");
            saida.println("# l2 instruction cache config, i.e., {<config>|dl2|none}");
            saida.println("-cache:il2                      " + caracs.get(i++));//39
            saida.println("");
            saida.println("# <MSG>");
            saida.println("# l2 instruction cache hit latency (in cycles)");
            saida.println("-cache:il2lat                     " + caracs.get(i++));//40
            saida.println("");
            saida.println("##############");
            saida.println("");
            saida.println("# flush caches on system calls");
            saida.println("-cache:flush                  " + caracs.get(i++));//41
            saida.println("");
            saida.println("# convert 64-bit inst addresses to 32-bit inst equivalents");
            saida.println("-cache:icompress              " + caracs.get(i++));//42
            i++;

            saida.println("");
            saida.println("");

            saida.println("##############################################################################");
            saida.println("##  MEMORIA");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# memory access latency (<first_chunk> <inter_chunk>)");
            saida.println("-mem:lat                        " + caracs.get(i++));//43
            saida.println("");
            saida.println("# memory access bus width (in bytes)");
            saida.println("-mem:width                        " + caracs.get(i++));//44

            saida.println("");
            saida.println("");

            saida.println("##############################################################################");
            saida.println("##  TLB");
            saida.println("##############################################################################");
            saida.println("");
            saida.println("# instruction TLB config, i.e., {<config>|none}");
            saida.println("-tlb:itlb              " + caracs.get(i++));//45
            saida.println("");
            saida.println("# data TLB config, i.e., {<config>|none}");
            saida.println("-tlb:dtlb              " + caracs.get(i++));//46
            saida.println("");
            saida.println("# *** Mudado");
            saida.println("# inst/data TLB miss latency (in cycles)");
            saida.println("-tlb:lat 		70");
            saida.println("");

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            return retorno;

        } finally {

            try {
                writer.close();
                saida.close();
            } catch (IOException ex) {
                Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            }

        }

        return "Sucesso";

    }

    public static String GetIn(String Comando){

        boolean taX = false;
        boolean taY = false;

        String nome = "vazio";
        String retorno = "ERRO";
        String[] aux = Comando.split(":&:");

        int clickX = Integer.parseInt(aux[0]);
        int clickY = Integer.parseInt(aux[1]);

        Point p = new Point();

        for (int i = 0; i < compsNaTela.size(); i++) compsNaTela.get(i).setisSelect(false);

        haveSelect = false;

        for (int i = compsNaTela.size() - 1; i >= 0; i--){

            taX = false;
            taY = false;

            p = compsNaTela.get(i).getLocal();

            if ((clickX > p.getX()) && (clickX < (p.getX() + compsNaTela.get(i).getHeight()))) taX = true;
            if ((clickY > p.getY()) && (clickY < (p.getY() + compsNaTela.get(i).getWidth()))) taY = true;

            if (taX && taY) nome = compsNaTela.get(i).getCompName();

        }

        if (conect1.equals("vazio") && !nome.equals("vazio")){

            conect1 = nome;
            retorno = "Sucesso:&:";

        }else if (!nome.equals("vazio") && !nome.equals("vazio")){

            String cone = conect1 + "::" + nome;
            conexoes.add(cone);

            retorno = "ConexaoEstabelecida:&:" + conect1 + "::" + nome;

            conect1 = "vazio";

        }

        return retorno;

    }

    public static String GetSelectType(String Comando){

        return compsNaTela.get(compsNaTela.size() - 1).getCompType();
        //else return "Nenhum Componente Selecionado";


    }

    public static String GetSelectCarac(String Comando){

        if (haveSelect){

            Vector vet = compsNaTela.get(compsNaTela.size() - 1).getCarac();
            String caracs = "";

            for (int i = 0; i < vet.size(); i++) caracs += (String)vet.get(i) + ":&:";

            return caracs;


        } else return "Nenhum Componente Selecionado";

    }

    public static String addComp(String Comando){

        String aux[] = Comando.split(":&:");
        Vector caracs = new Vector();

        Point ip = new Point(Integer.parseInt(aux[4]), Integer.parseInt(aux[5]));

        for (int i = 6; i < aux.length-1; i++){
            
            if (aux[i].equals("nulo")) caracs.add("<null>");
            else caracs.add(aux[i]);

        }

        compsNaTela.add(new inScreen(null, false, aux[0], aux[1], null, ip, Integer.parseInt(aux[2]), Integer.parseInt(aux[3])));
        compsNaTela.get(compsNaTela.size() - 1).setCarac(caracs);

        //System.out.println("Componente adicionado: " + compsNaTela.get(compsNaTela.size() - 1).getCompName() + " " + compsNaTela.get(compsNaTela.size() - 1).getHeight());

        return "Sucesso";

    }

    public static String zeraConect(String Comando){

        conect1 = "vazio";
        return "Sucesso";

    }

    public static String descelecionar(String Comando){

        if (compsNaTela.size() > 1) compsNaTela.get(compsNaTela.size() - 1).setisSelect(false);
        return "Sucesso";

    }

    public static String selecionar(String Comando){

        haveSelect = true;
        String nome = Comando.split(":&:")[0];
        inScreen atual = new inScreen();
        int pos = -1;

        for (int i= 0; i < compsNaTela.size(); i++){

            if (compsNaTela.get(i).getCompName().equals(nome)){

                pos = i;
                atual = compsNaTela.get(i);


            }

        }

        compsNaTela.remove(pos);
        compsNaTela.add(atual);

        return "Sucesso";

    }

    public static String setLocal (String Comando){

        String retorno = "Nenhum componente foi alterado!";
        String aux[] = Comando.split(":&:");
        Point p = new Point(Integer.parseInt(aux[1]), Integer.parseInt(aux[2]));

        for (int i = 0; i < compsNaTela.size(); i++){

            if (compsNaTela.get(i).getCompName().equals(aux[0])){

                compsNaTela.get(i).setLocal(p);
                retorno = "Sucesso";
                break;

            }

        }

        return retorno;

    }

    public static String newDir (String Comando){

        dir = Comando.split(":&:")[0];
	if (!dir.split("")[dir.split("").length - 1].equals("/")) dir += "/";

	File file = new File(dir);
        if (!file.exists()) file.mkdir();

     //   dir = "/home/vipro/testes/";

        return "Sucesso";

    }

    public static String getBarrIn(String comp){

	for (int i = 0; i < conexoes.size(); i++){

	    if (conexoes.get(i).split("::")[0].equals(comp)){

		for (int j = 0; j < compsNaTela.size(); j++){

		    if (conexoes.get(i).split("::")[1].equals(compsNaTela.get(j).getCompName()) && compsNaTela.get(j).getCompType().equals("barramento")) return compsNaTela.get(j).getCompName();

		}

	    }else if (conexoes.get(i).split("::")[1].equals(comp)){

		for (int j = 0; j < compsNaTela.size(); j++){

		    if (conexoes.get(i).split("::")[0].equals(compsNaTela.get(j).getCompName()) && compsNaTela.get(j).getCompType().equals("barramento")) return compsNaTela.get(j).getCompName();

		}

	    }

	}

	return "ERRO";

    }

    public static String removeSelectParcial(String Comando){

        compsNaTela.remove(compsNaTela.size() - 1);
        return "Sucesso";

    }

    public static String removeSelectTotal(String Comando){

        String name = compsNaTela.get(compsNaTela.size() - 1).getCompName();

        for (int i = 0; i < compsNaTela.size(); i++){

            if (conexoes.get(i).split("::")[0].equals(name) || conexoes.get(i).split("::")[1].equals(name)) conexoes.remove(i);

        }

        compsNaTela.remove(compsNaTela.size() - 1);
        return "Sucesso";

    }

    public static String GetFiles(String Comando){

        String retorno = "Sucesso";

        try {

            try {

                Process ret = Runtime.getRuntime().exec("cp /home/vipro/VIPRO-MPv0.2/VIPRO-MP/input256.ppm " + dir);
                ret.waitFor();
                ret = Runtime.getRuntime().exec("cp /home/vipro/VIPRO-MPv0.2VIPRO-MP/sharedmemory.cpp " + dir);
                ret.waitFor();
                ret = Runtime.getRuntime().exec("cp /home/vipro/VIPRO-MPv0.2VIPRO-MP/sharedmemory.hpp " + dir);
                ret.waitFor();

                ret.destroy();

            } catch (InterruptedException ex) {
                Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            }

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            retorno = "ERRO";

        }

        return retorno;

    }

    public static String CommandMake(String Comando){

        String retorno = "Sucesso";

        try {
            
            Process ret = Runtime.getRuntime().exec("chmod +x " + dir + "Script");
            ret.waitFor();
            ret = Runtime.getRuntime().exec("chmod 777 " + dir + "Script");
            ret.waitFor();
            ret = Runtime.getRuntime().exec("make", null, new File(dir));
            System.out.println(new File(dir));
            BufferedReader reader= new BufferedReader(new InputStreamReader(ret.getErrorStream()));
            BufferedReader reader1= new BufferedReader(new InputStreamReader(ret.getInputStream()));
            
            String s;
            do{
                s= reader.readLine();
                System.out.println(s);
             //   s= reader1.readLine();
             //   System.out.println(s);
            }while (s!=null);
            int retval= ret.waitFor() ;
            System.out.println(retval);
            if (retval==0) retorno = "Sucesso";
            else retorno = "ERRO";

            ret.destroy();

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            retorno = "ERRO";

        } catch (InterruptedException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            retorno = "ERRO";

        }

        return retorno;

    }

    public static String Executar(String Comando){

        String retorno = "Sucesso";

        try {

            Process ret = Runtime.getRuntime().exec("./Script", null, new File(dir));
            BufferedReader reader= new BufferedReader(new InputStreamReader(ret.getErrorStream()));
            BufferedReader reader1= new BufferedReader(new InputStreamReader(ret.getInputStream()));
            
            String s;
            do{
                s= reader.readLine();
               System.out.println(s);
              //  s= reader1.readLine();
            //    System.out.println(s);
            }while (s!=null);
            int retval= ret.waitFor() ;
            if (retval== 0) retorno = "Sucesso";
            else retorno = "ERRO no executavel";

            ret.destroy();

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            retorno = "ERRO exceção 1";

        } catch (InterruptedException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            retorno = "ERRO exceção 2";

        }

        return retorno;

    }

    public static String Save (String Comando){

        String retorno = "Sucesso";
        File saveFile = new File(dir + "Projeto.vmp");

        FileWriter writer = null;
        PrintWriter saida = null;

        String comp = "";

        try {

            writer = new FileWriter(saveFile);
            saida = new PrintWriter(writer);

            inScreen aux = new inScreen();
            Vector caracs = new Vector();

            saida.println(dir);
            saida.println("Comps");
            for (int i = 0; i < compsNaTela.size(); i++){

                aux = compsNaTela.get(i);

                comp = aux.getCompType() + " " + aux.getCompName() + " " + (int)aux.getLocal().getX() + " " + (int)aux.getLocal().getY() + " " + aux.getHeight() + " " + aux.getWidth();
                caracs = aux.getCarac();

                for (int j = 0; j < caracs.size(); j++) comp += " " + caracs.get(j);

                saida.println(comp);

            }

            saida.println("Conects");
            for (int i = 0; i < conexoes.size(); i++) saida.println(conexoes.get(i));

        } catch (IOException ex) {

            Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            retorno = "ERRO";

        } finally {
            
            try {
                writer.close();
                saida.close();
            } catch (IOException ex) {
                Logger.getLogger(Ambiente.class.getName()).log(Level.SEVERE, null, ex);
            }

        }

        return retorno;

    }

    public static String Open (String Comando){

        String retorno = "";
        String[] leu;
        String auxLeu = "";
        
        inScreen aux = new inScreen();
        Vector caracs = new Vector();

        boolean comp = true;

        compsNaTela = new Vector<inScreen>();
        conexoes = new Vector<String>();

        try {

            BufferedReader in = new BufferedReader(new FileReader(Comando.split(":&:")[0]));

            /*dir =*/ in.readLine();
            if(!in.readLine().equals("Comps")) return "Arquivo Corrompido ou Invalido";

            while (in.ready()){

                caracs = new Vector();
                auxLeu = in.readLine();

                if(auxLeu.equals("Conects")) comp = false;

                else if (comp){

                    //System.out.println("\n");
                    leu = auxLeu.split(" ");
                    aux = new inScreen(null, false, leu[1], leu[0], null, new Point(Integer.parseInt(leu[2]), Integer.parseInt(leu[3])), Integer.parseInt(leu[4]), Integer.parseInt(leu[5]));
                    retorno += auxLeu + ":&:";

                    for (int i = 6; i < leu.length; i++){
                        System.out.println("i ="+i+" value="+ leu[i]);
                        if (i == 20) caracs.add(leu[i++] + " " + leu[i++] + " " + leu[i++] + " " + leu[i]);                     
                        else if (i == 60) caracs.add(leu[i++] + " " + leu[i++] + " " + leu[i++] + " " + leu[i]);
                        else if (i == 26) caracs.add(leu[i++] + " " + leu[i]);
                        else if (i == 54) caracs.add(leu[i++] + " " + leu[i]);
                        else caracs.add(leu[i]);                      

                    }

                    aux.setCarac(caracs);
                    /*System.out.println("DAKI---------------------------");
                    for (int i = 0; i < caracs.size(); i++) System.out.println(i + " " + caracs.get(i));
                    System.out.println("ATE aKI-------------------------");*/

                    compsNaTela.add(aux);

                } else {

                    conexoes.add(auxLeu);
                    retorno += auxLeu + ":&:";

                }

            }

            in.close();

        } catch(IOException e) {

            return "Arquivo Corrompido ou Invalido";

        }

        return retorno;

    }

    public static String limpar(String Comando){

        String retorno = "Sucesso";

        compsNaTela = new Vector<inScreen>();
        conexoes = new Vector<String>();

        return retorno;

    }

    public static String variar(String Comadndo){


        newDir("/home/vipro/testes/TestesExaustivos");        

        File outPut = new File("/home/vipro/testes/TestesExaustivos/TempoC.txt");

        long TempoInicTot = System.currentTimeMillis();
        long TempoInicParc = System.currentTimeMillis();

        long TempoFinTot = 0;
        long TempoFinPar = 0;

        String retorno = "Sucesso";
        Vector caracs = new Vector();

        double potencia = 0.0;
        double desempenho = 0.0;

        File Log = new File("/home/vipro/testes/TestesExaustivos/dump");
        PrintWriter LogOut = null;

        OtimaArquitetura.setDesempenho(999999999.9);
        BufferedReader in = null;
        File dump = null;

        try{

        FileWriter writer = new FileWriter(outPut);
        PrintWriter saida = new PrintWriter(writer);
        LogOut = new PrintWriter(new FileWriter(Log));
        // MSO: Ok, now the parameters will be configurable
        /* Parameters: 
        minMem, maxMem: memory component caracs.set(caracs.size() - 1, mem);
        minFun, maxFun (integer unit): #28 
        minIL1, maxIL1: #33
        minDL1, maxDL1: #35
        minScalar, maxScalar: #19 (ifqsize), #21 (decode), #22 (issue), #25 (commit)
                
        */
        int cont=0;  
        for (int mem = minmaxvariar[0][0]; mem <= minmaxvariar[0][1]; mem = mem * 2){//4 - 16

            for (int unFun = minmaxvariar[1][0]; unFun <= minmaxvariar[1][1]; unFun++){ //1 - 8

                for (int cacheil = minmaxvariar[2][0]; cacheil <= minmaxvariar[2][1]; cacheil = cacheil * 2){ //256 - 262144

                    for (int cachedl = minmaxvariar[3][0]; cachedl <= minmaxvariar[3][1]; cachedl = cachedl * 2){ //64 - 65536
    
                        for (int scalar = minmaxvariar[4][0]; scalar <= minmaxvariar[4][1]; scalar++){ //64 - 65536

                            TempoInicParc = System.currentTimeMillis();
                            System.out.println("exec num: " + cont);
                            newDir("/home/vipro/testes/TestesExaustivos/Saida_" + mem + "_" + unFun + "_" + cacheil + "_" + cachedl + "_" + cont);

                            for (int k = 0; k < compsNaTela.size(); k++){

                                if (compsNaTela.get(k).getCompType().equals("processador")){

                                    caracs = compsNaTela.get(k).getCarac();
                                    //minScalar, maxScalar: #19 (ifqsize), #21 (decode), #22 (issue), #25 (commit)
                                    caracs.set(19, scalar);
                                    caracs.set(21, scalar);
                                    caracs.set(22, scalar);
                                    caracs.set(25, scalar);
                                                                        
                                    caracs.set(28, unFun);
                                    
                                    caracs.set(33, "il1:" + cacheil + ":32:1:l");
                                    caracs.set(35, "dl1:" + cachedl + ":32:4:l");

                                    compsNaTela.get(k).setCarac(caracs);

                                }else if (compsNaTela.get(k).getCompType().equals("memoria")){

                                    caracs = compsNaTela.get(k).getCarac();

                                    caracs.set(caracs.size() - 1, mem);

                                    compsNaTela.get(k).setCarac(caracs);

                                }



                            }

                            Save("");

                            //System.out.println("Dir: " + dir);

                            makeFile("");
                            Main("");
                            Scripts("");
                            makeConfigure("");
                            GetFiles("");
                            CommandMake("");
                            Executar("");                          

                            potencia = 0.0;
                            desempenho = 0.0;

                            try{

                                dump = new File("/home/vipro/testes/TestesExaustivos/Saida_" + mem + "_" + unFun + "_" + cacheil + "_" + cachedl + "_" + cont + "/dumps");
                                in = new BufferedReader(new FileReader(dump));
                                //TODO: Fixed number of processors  2!! Must be variable!
                                for(int procs = 0; procs < 2; procs++){

                                    String auxLeu = in.readLine();
                                    String leu[];
                                    // read the first sim_cycle
                                    for(leu = auxLeu.split(" "); !leu[0].equals("sim_cycle"); leu = auxLeu.split(" "))
                                        auxLeu = in.readLine();

                                    int pos;

                                    for(pos = 1; leu[pos].trim().equals(""); pos++);
                                    // read the second sim_cycle, the last processor to finish  the execution
                                    // TODO: and in the case of N processors!!
                                    if(Double.parseDouble(leu[pos]) / Math.pow(10, 8)> desempenho)
                                        desempenho = Double.parseDouble(leu[pos]) / Math.pow(10, 8);

                                    auxLeu = in.readLine();
                                    // MSO: changed to avg_total_power_cycle_cc2 because it is a more realistic measure
                                    for(leu = auxLeu.split(" "); !auxLeu.split(" ")[0].equals("avg_total_power_cycle_cc2"); leu = auxLeu.split(" "))
                                        auxLeu = in.readLine();

                                    for(pos = 1; leu[pos].trim().equals(""); pos++);
                                    // The total power is the sum of the two processors
                                    if(Double.parseDouble(leu[pos])> 0.0){
                                        potencia += Double.parseDouble(leu[pos]) / 100000.00;
                                    } else {
                                        LogOut.println("ERRO, potencia acima do suportado. Individuo " + 3 + " geracao " + 3 + " processador " + procs);
                                        potencia += 999999999 / 2;
                                    }

                                }

                            } catch(Exception e) {
                                LogOut.println ("ERRO ao ler dump " + cont);
                            } finally {
                                in.close();
                            }

                            if(potencia <= 0.0 || desempenho <= 0.0){

                                potencia = 999999999;
                                desempenho = 999999999;

                            }

                            potencia /= 2;

                            TempoFinPar = System.currentTimeMillis();

                            if ((potencia + desempenho) < OtimaArquitetura.getDesempenho()){
                                OtimaArquitetura.setFitness((desempenho + potencia));
                                OtimaArquitetura.setDesempenho(desempenho * Math.pow(10, 8));
                                OtimaArquitetura.setPotencia(potencia * 100000);
                                OtimaArquitetura.setGerac(cont);
                            }

                            LogOut.println(("Saida_" + mem + "_" + unFun + "_" + cacheil + "_" + cachedl + "_" + scalar+"_"+ cont +":"+desempenho * Math.pow(10, 8)) + ":" + (potencia * 100000.00));
                            LogOut.flush();

                            cont++;                        

                        }
                    }

                }

            }

        }

        TempoFinTot = System.currentTimeMillis();
        LogOut.println("\nMelhor arquitetura: " + OtimaArquitetura.getGerac() + " com um fitness de: " + OtimaArquitetura.getDesempenho());
        LogOut.println("Sendo:");
        LogOut.println("Potência: " + OtimaArquitetura.getPotencia());
        LogOut.println("Desempenho: " + OtimaArquitetura.getDesempenho());

        saida.close();
        writer.close();
        LogOut.close();

        return retorno;

        } catch(IOException e) {

            System.out.println(e);
            return "Arquivo Corrompido ou Invalido";

        }

    }

    public static String otimizar (String Comando){

        return Otimizar.otimizarFunc(compsNaTela, conexoes);

    }

    public static void setComp (Vector<inScreen> individuo){

        compsNaTela = new Vector<inScreen>(individuo);

    }

    public static void ShowComp(){

        //System.out.println("\n\n");
        for (int i = 0; i < compsNaTela.size(); i++){

            System.out.println(i + " " + compsNaTela.get(i) + compsNaTela.get(i).getCompType());

            if (compsNaTela.get(i).getCompType().equals("processador")){

                for (int z = 0; z < compsNaTela.get(i).getCarac().size(); z++)
                    System.out.println("    - " + z + " " + compsNaTela.get(i).getCarac().get(z));

            }


        }
        //System.out.println("\n\n");

    }

    public static String getDesc(){

        String Desc = "";

        for (int i = 0; i < compsNaTela.size(); i++){

            //System.out.println(i + " " + compsNaTela.get(i) + compsNaTela.get(i).getCompType());
            Desc += i + " " + compsNaTela.get(i).getCompName() + " " + compsNaTela.get(i).getCompType() + "\n";

            //if (compsNaTela.get(i).getCompType().equals("processador")){

                for (int z = 0; z < compsNaTela.get(i).getCarac().size(); z++)
                    //System.out.println("    - " + z + " " + compsNaTela.get(i).getCarac().get(z));
                    Desc += "    - " + getCaracByPos(z, compsNaTela.get(i).getCompType()) + " " + compsNaTela.get(i).getCarac().get(z) + "\n";

            //}


        }

        return Desc;

    }

    public static String getDescText(){

        String Desc = "";

        for (int i = 0; i < compsNaTela.size(); i++){

            //System.out.println(i + " " + compsNaTela.get(i) + compsNaTela.get(i).getCompType());
            Desc += i + ":" + compsNaTela.get(i).getCompName() + ":" + compsNaTela.get(i).getCompType() ;

            if (compsNaTela.get(i).getCompType().equals("processador")){

                for (int z = 11; z <= 45; z++)
                    //System.out.println("    - " + z + " " + compsNaTela.get(i).getCarac().get(z));
                    Desc += ":" + compsNaTela.get(i).getCarac().get(z)+":";
            }


        }

        return Desc;

    }

        
    public static String getCaracByPos(int pos, String type){

        if (type.equals("processador")){

            if (pos == 0) return       "random number generator seed (0 for timer seed):..............";
            else if (pos == 1) return  "initialize and terminate immediately:.........................";
            else if (pos == 2) return  "restore EIO trace execution from <fname>:.....................";
            else if (pos == 3) return  "redirect simulator output to file (non-interactive only):.....";
            else if (pos == 4) return  "redirect simulated program output to file:....................";
            else if (pos == 5) return  "simulator scheduling priority:................................";
            else if (pos == 6) return  "maximum number of inst's to execute:..........................";
            else if (pos == 7) return  "number of insts skipped before timing starts:.................";
            else if (pos == 8) return  "generate pipetrace, i.e., <fname|stdout|stderr> <range>:......";
            else if (pos == 9) return  "profile stat(s) against text addr's (mult uses ok):...........";
            else if (pos == 10) return "operate in backward-compatible bugs mode (for testing only):..";
            else if (pos == 11) return ""+ pos;
            else if (pos == 12) return ""+ pos;
            else if (pos == 13) return ""+ pos;
            else if (pos == 14) return ""+ pos;
            else if (pos == 15) return ""+ pos;
            else if (pos == 16) return ""+ pos;
            else if (pos == 17) return ""+ pos;
            else if (pos == 18) return ""+ pos;
            else if (pos == 19) return ""+ pos;
            else if (pos == 20) return ""+ pos;
            else if (pos == 21) return ""+ pos;
            else if (pos == 22) return ""+ pos;
            else if (pos == 23) return ""+ pos;
            else if (pos == 24) return ""+ pos;
            else if (pos == 25) return ""+ pos;
            else if (pos == 26) return ""+ pos;
            else if (pos == 27) return ""+ pos;
            else if (pos == 28) return ""+ pos;
            else if (pos == 29) return ""+ pos;
            else if (pos == 30) return ""+ pos;
            else if (pos == 31) return ""+ pos;
            else if (pos == 32) return ""+ pos;
            else if (pos == 33) return ""+ pos;
            else if (pos == 34) return ""+ pos;
            else if (pos == 35) return ""+ pos;
            else if (pos == 36) return ""+ pos;
            else if (pos == 37) return ""+ pos;
            else if (pos == 38) return ""+ pos;
            else if (pos == 39) return ""+ pos;
            else if (pos == 40) return ""+ pos;
            else if (pos == 41) return ""+ pos;
            else if (pos == 42) return ""+ pos;
            else if (pos == 43) return ""+ pos;
            else if (pos == 44) return ""+ pos;
            else if (pos == 45) return ""+ pos;
            else if (pos == 46) return ""+ pos;
            else if (pos == 47) return ""+ pos;
            else if (pos == 48) return ""+ pos;
            else if (pos == 49) return ""+ pos;

            else return "ERRO";

        }

            return "" + pos;

    }


}
