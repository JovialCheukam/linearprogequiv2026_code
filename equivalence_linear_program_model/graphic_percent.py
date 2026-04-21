import pandas as pd
import subprocess

# This routine converts seconds to a string in HH:MM:SS format.
def convert_secs_to_hour_min_sec(seconds):
    hours, remainder = divmod(seconds, 3600)
    hours = seconds // 3600
    remainder = seconds - hours * 3600
    minutes = remainder // 60
    seconds_ = remainder - minutes * 60
    return "{:02}:{:02}:{:02}".format(hours, minutes, seconds_)


luby = True

def get_all_data_results(data,timeout_min):
    Directory = "/Users/admin/Documents/POSTDOC_MichealMorin/CODE_PostDoc/env_code_postdoc_jovial/for_fir/code_exp_postdoc_jov_/results_exp/"
    if luby :
        df = pd.read_csv(Directory+"results_of_exp_luby_" + data + ".csv")
    else :
        df = pd.read_csv(Directory+"results_of_experiments_" + data + ".csv")
    i = 0
    for model in df['model']:
        if df.loc[i,'instance name'] == 'netlib/':
            df.loc[i,'instance name'] = 'bnl1'
        if model in ['algo','algo_enhanced', 'luby_test_small_restart', 'luby_test_medium_restart','luby_test_full_restart']:
            df.loc[i,'model'] = model
            df.loc[i,'solver'] = model
            if df.loc[i,'type of instance'] == 'eq':
               df.loc[i,'type of instance'] = 'equal by permutation'
            if df.loc[i,'type of instance'] == 'not_eq':
               df.loc[i,'type of instance'] = 'not equal by permutation'
            if df.loc[i,"result of the test"] == 'not-equivalent':
               df.loc[i,"result of the test"] = 'not equivalent'
           
        i = i + 1
    j = 0
    for result in df["result of the test"]:
        if result == ["unknown due to timeout", "unknow",'error:-should-be-not-equivalent','error:-should-be-equivalent']:
            df.loc[j,"execution time"] = convert_secs_to_hour_min_sec(timeout_min*60)
        j = j + 1
    return df


def data_clustered_by_size_num(ith,num_cluster,data,set_web, set_inst,timeout):
    data_fram = get_all_data_results(data,timeout)
    assert(ith >= 1)
    assert(ith <= num_cluster)
    meta_data = pd.read_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')
    size_cluster = len(meta_data)//num_cluster
    print("size_cluster = ", size_cluster)
    start_index = (ith-1)*size_cluster
    end_index = ith*size_cluster
    min_size = meta_data.loc[start_index, 'size']
    max_size = meta_data.loc[end_index-1, 'size']
    if ith == num_cluster:
        return data_fram[(data_fram['size'] >= min_size)].reset_index(), size_cluster + len(meta_data) % num_cluster
    else:
        return data_fram[(data_fram['size'] >= min_size) & (data_fram['size'] < max_size)].reset_index(), size_cluster

def type_cluster(ith_cluster,num_insts,data_name):
    if ith_cluster == 0:
        return 'Test of ' + str(num_insts) + ' small instances of ' + data_name
    elif ith_cluster == 1:
        return 'Test of ' + str(num_insts) + ' average instances of ' + data_name
    else:
        return 'Test of ' + str(num_insts) +' large instances of ' + data_name



preambule1 = "\\documentclass{book}\n\\usepackage[landscape,a3paper,left=0.5cm,right=1in,top=1in,"
preambule2 = "bottom=1in]{geometry}\n\\usepackage{amsmath}\n\\usepackage{xcolor}\n\\usepackage{tikz}\n"
preambule3 = "\\usepackage{pgfplots}\n\\usepackage{booktabs}\n\\usepackage{multirow}\n\\usepackage{amsmath,"
preambule4 = "amssymb}\n\n\\begin{document}\n\n\\vspace{2cm}\n\n"
preambule5 = "\\begin{figure}\\pgfplotsset{samples=10}\n\\centering\n\\begin{tabular}{lr}"


def end_graphic(ith_grahic):
    return  "\n\\end{tabular}\\\\\n\\ref{named"+str(ith_grahic)+"}\\end{figure}\n\n"


def first_head_env_graphi(title,type_axis,xlabel,ylabel,list_of_graphs_legend,ith_grahic):
    return "\\begin{tikzpicture}\n\\begin{"+type_axis+"}[scaled ticks=false, xscale=1.5,legend columns = 5,legend entries=" + list_of_graphs_legend+", legend to name = named"+str(ith_grahic)+ ",title style={at={(0.38,1)}},\ntitle={" + title + "},xlabel=" + xlabel +",ylabel=" + ylabel+"]\n"

def seconds_head_env_graphi(title,type_axis,xlabel,ylabel,p):
    if 2 == p:
        ylabel_written = ",ylabel=" + ylabel
    else:
        ylabel_written =""
        
    return "\\begin{tikzpicture}\n\\begin{"+type_axis+"}[scaled ticks=false,xscale=1.5,title style={at={(0.38,1)}}, \ntitle={" + title + "},xlabel=" + xlabel+ylabel_written+"]\n"

def add_function(function,min_val,max_val):
    return "\\addplot[domain="+min_val+":"+max_val+"] {"+ function + "};\n"
    
def add_plot(color,mark,coordinates):
    return "\\addplot[color="+color+",mark="+mark+"] coordinates {"+ coordinates + "};\n"   

def end_env_graphic(type_axis):
    return "\\end{"+type_axis+"}\n\\end{tikzpicture}\n"


def compute_time(time_string):
    format_seconds = time_string.split(':')
    time_second = float(format_seconds[0])*3600 + float(format_seconds[1])*60 + float(format_seconds[2])
    return time_second//60

def get_num_inst0(df):
    l = len(df)
    inst = df.loc[0,'instance name']
    k = 1
    for i in range(l):
        if df.loc[i,'instance name'] != inst:
            k = k + 1
            inst = df.loc[i,'instance name']
    return k

def get_num_inst(df):
    #print(df.drop_duplicates(subset=['instance name']))

    return len(df['instance name'].drop_duplicates())

def compute_proportion(df,frozen_time_min,type_inst,results_test,solver,model,num_inst,timeout):
    portion = 0
    
    df_time = df[(df['solver'] == solver) & (df['type of instance'] == type_inst) & (df['result of the test'] == results_test) & (df['model'] == model)]
    #if model == 'algo_enhanced' and type_inst == 'not equal by permutation':
    #    print('len(df_time) = ',len(df_time))
        #print(df_time['instance name'])
        
    if df_time.empty:
        return 0
    #num_inst = get_num_inst(df_time.reset_index())
    #print("num_inst = ", num_inst)
    #num_inst = 31
    for time_string in df_time['execution time']:
        minutes = compute_time(time_string)
        if frozen_time_min == timeout:
            if minutes < frozen_time_min:
               portion = portion + 1            
        else:
            if minutes <= frozen_time_min:
               portion = portion + 1
    if luby :
        if solver == 'luby_test_small_restart':
            portion = portion/10
    return round((portion / num_inst)* 100,2)
    
##############################################################################################################################################
###################################### gen_graph_proportion ( NOT WITH LUBY FOR NOW)
def graph_coordinates_success_time(df,type_inst,num_inst,results_test,model,solver,color,mark,file):
    prop_update = -1
    coordinates = ""
    if model in ['algo','algo_enhanced'] and (type_inst == 'equal by permutation' or type_inst == 'not equal by permutation'):
            coordinates = "(0,0)\n"
    else :
        for frozen_time_min in range(1,1440,10):
            x = str(frozen_time_min)
            y = compute_proportion(df,frozen_time_min,type_inst,results_test,solver,model,num_inst)
            if y != prop_update and frozen_time_min < 1431:
                coordinates = coordinates + "("+ x + "," + str(y) +")\n"
                prop_update = float(y)
            if frozen_time_min == 1431:
                coordinates = coordinates + "("+ x + "," + str(y) +")\n"
    coordonate = add_plot(color,mark,coordinates)
    file.write(coordonate)


def title_(type_inst):
    if type_inst == 'not equal by permutation':
        return '\\textsc{Non-Equiv + Permut}'
    else:
        return '\\textsc{Equiv + Permut}'

def color_mark(models,solvers,algo_enhanced):
    avail_res = {}
    for model in models.keys():
        for solver in solvers.keys():
            if model not in algo_enhanced and solver not in algo_enhanced:
                avail_res[(model,solver)]= [models[model],solvers[solver]]
    avail_res[(algo_enhanced[0],algo_enhanced[0])] = ['brown','oplus']
    avail_res[(algo_enhanced[1],algo_enhanced[1])] = ['red','otimes']
    return avail_res
    

def gen_coordinate(df,type_inst,num_inst,results_test,available_res,file):
    for (model,solver) in available_res.keys():
            color = available_res[(model,solver)][0]
            mark = available_res[(model,solver)][1]
            graph_coordinates_success_time(df,type_inst,num_inst,results_test,model,solver,color,mark,file)
        
def graphic_model(df,type_axis,type_inst,results_test,models,solvers,p,file):
        title = title_(type_inst)
        ylabel = "\\% of solved instances"
        xlabel = "Time (min)"

        graphic_head = seconds_head_env_graphi(title,type_axis,xlabel,ylabel,p)
        file.write(graphic_head)
        available_res = color_mark(models,solvers,['algo','algo\\_enhanced'])
        gen_coordinate(df,type_inst,num_inst,results_test,available_res,file)
        end_env = end_env_graphic(type_axis)
        file.write(end_env)


def write_legend(models_,solvers_):
    i = 0
    legend ="{"
    available_res = color_mark(models_,solvers_,['algo','algo\\_enhanced'])
    for (model,solver) in available_res.keys():
        if model in ['algo','algo\\_enhanced']:
            name = model
        else:
            name = model + " + " + solver
        if i == 0:
               legend = legend + name
        elif i == len(available_res)-1:
                legend = legend + ";," + name + "}"
        else:
                legend = legend + ";," + name
        i = i + 1
    return legend


def gen_graph_proportion(df,instance_difficulty,models,solvers,models_string,solvers_string,num_inst,
                        file,ith_grahic,list_frozen_time_min):
    #with open(directory+"/graphics.tex", "w") as file: 
    #    file.write(preambule1)
    #    file.write(preambule2)
    #    file.write(preambule3)
    #    file.write(preambule4)
        ylabel = "\\% of solved instances"
        xlabel = "Time (min)"

        type_inst_res_list = [('not equal by permutation','not equivalent')]
        file.write(preambule5)
        list_of_graphs_legend = write_legend(models_string,solvers_string)
        title = "Equiv + Permut"
        type_axis = "axis"
        first_head = first_head_env_graphi(title,type_axis,xlabel,ylabel,list_of_graphs_legend,ith_grahic)
        file.write(first_head)
        available_res = color_mark(models,solvers,['algo','algo_enhanced'])
        gen_coordinate(df,'equal by permutation',num_inst,'equivalent',available_res,file)
        end_env = end_env_graphic(type_axis)
        file.write(end_env+"&")
        i = 1
        for (type_inst,results_test) in type_inst_res_list:
            graphic_model(df,type_axis,type_inst,results_test,models,solvers,i,file)
            if i == 1 :
                file.write("\\\\\n")
            if i == 2 :
                file.write(" & ")
            i = i + 1
        file.write('\n')
        file.write(end_graphic(ith_grahic))
        file.write('\n\\begin{tabular}{ll}\n\n')
        gen_table_proportion(df,instance_difficulty,list_frozen_time_min,models,solvers,models_string,
                         solvers_string,'equal by permutation',num_inst,'equivalent',file)
        file.write(" & ")
        i = 1
        for (type_inst,results_test) in type_inst_res_list:
            gen_table_proportion(df,instance_difficulty,list_frozen_time_min,models,solvers,models_string,
                         solvers_string,type_inst,num_inst,results_test,file)
            if i % 2 != 0 :
                file.write("\\\\\\\\\n")
            else :
                file.write(" & ")
            i = i + 1
        file.write('\n\\end{tabular}\n\n')
        file.write("\\newpage")
##############################( END OF CODE FOR gen_graph_proportion ( NOT WITH LUBY FOR NOW)#####################################################

def write_name(model_solver_algo,type_inst):
    if model_solver_algo == 'algo':
        if type_inst in ['equal by permutation','not equal by permutation']:
            return '\\textsc{Algo}'
    elif model_solver_algo == 'luby\\_test\\_small\\_restart':
        return '\\textsc{LubyTestSmallRestart}'
    elif model_solver_algo == 'luby\\_test\\_full\\_restart':
        return '\\textsc{LubyTestFullRestart}'
    elif model_solver_algo == 'luby\\_test\\_medium\\_restart':
        return '\\textsc{LubyTestMediumRestart}'
    elif model_solver_algo == 'algo\\_enhanced':
        return '\\textsc{AlgoEnhanced}'
    elif model_solver_algo == 'quadratic\\_model':
        return '\\textsc{QuadraMod}'
    elif model_solver_algo == 'theorem\\_model':
        return '\\textsc{TheoMod}'
    else:
        pass

def write_model_solver(type_inst,list_models,list_solvers,list_models_string,list_solvers_string):
    l_m = len(list_models)
    l_s = len(list_solvers)
    if luby :
        algos = ['algo','algo\\_enhanced','luby\\_test\\_small\\_restart','luby\\_test\\_full\\_restart','luby\\_test\\_medium\\_restart']
    else :
        algos = ['algo','algo\\_enhanced']
    list_models_solver = []
    for i in range(l_m):
        for j in range(l_s):
            if list_models_string[i] in algos and list_solvers_string[j] not in algos:
                pass
            elif list_models_string[i] not in algos and list_solvers_string[j] in algos:
                pass
            elif list_models_string[i] in algos and list_solvers_string[j] in algos:
                name = write_name(list_models_string[i],type_inst)
                if (name,list_models[i],list_models[i]) not in list_models_solver:
                    list_models_solver.append((name,list_models[i],list_models[i]))
            else:
                name = write_name(list_models_string[i],type_inst) + " + " + list_solvers_string[j]
                list_models_solver.append((name,list_models[i],list_solvers[j]))
    return list_models_solver

def evaluate_proportion(df,list_frozen_time_min,type_inst,num_inst,results_test,list_models_solver,timeout):
    models_solvers = {}
    for (model_solver,_,_) in list_models_solver:
        models_solvers[model_solver] = []
    k = -1
    for (model_solver,model,solver) in list_models_solver:
        
        k = k + 1
        for frozen_time_min in list_frozen_time_min:
            models_solvers[model_solver].append([compute_proportion(df,frozen_time_min,type_inst,
                                                         results_test,solver,model,num_inst,timeout),k])
        models_solvers[model_solver].append([round(100-compute_proportion(df,frozen_time_min,type_inst,
                                                         results_test,solver,model,num_inst,timeout),2),k+1])
        
    l = len(list_frozen_time_min)
    sorter = {}
    for i in range(l+1):
        sorter[i] = []
    for i in range(l+1):
        for (model_solver,model,solver) in list_models_solver:
            sorter[i].append(models_solvers[model_solver][i])
    for i in range(l+1):
        sorter[i] = sorted(sorter[i])
    for i in range(l):
        for (model_solver,model,solver) in list_models_solver:
            if models_solvers[model_solver][i][0] == sorter[i][-1][0]:
                models_solvers[model_solver][i][1]= 'bf'
    
    for (model_solver,model,solver) in list_models_solver:
            if models_solvers[model_solver][l][0] == sorter[l][0][0]:
                models_solvers[model_solver][l][1]= 'bf'
    return models_solvers

def write_proportion(row_of_proportions):
    
    proportions =""
    for (proportion,best_or_not) in row_of_proportions[:-1]:
        if best_or_not == 'bf':
           proportions = proportions + ' & \\bf ' + str(proportion)
        else :
           proportions = proportions + ' & ' + str(proportion)
    if row_of_proportions[-1][1] == 'bf':
        time_out_percent = ' \\bf ' + str(row_of_proportions[-1][0])
    else :
        time_out_percent = str(row_of_proportions[-1][0])
    
    return proportions,time_out_percent

def write_frozen_time(list_frozen_time_min):
    frozen_time_min_string = ""
    for frozen_time_min in list_frozen_time_min:
        if frozen_time_min != list_frozen_time_min[-1]:
           frozen_time_min_string = frozen_time_min_string + '  $\\leq ' + str(frozen_time_min) + '$ &'
        else:
          frozen_time_min_string = frozen_time_min_string + '  $ < ' + str(frozen_time_min) + '$ &'
            
    return frozen_time_min_string 

def dict_keys_in_list(dictionary):
    list_keys = []
    for key in dictionary.keys():
        list_keys.append(key)
    return sorted(list_keys)
    
    

def make_dir_for_two_models(models,solvers,rank_cluster):
    directory = 'graphics/'
    for model in models.keys():
        directory = directory +'_'+ model
    for solver in solvers.keys():
        directory = directory +'_'+ solver
    directory = directory + '_inst_'+str(rank_cluster)
    # Execute a bash command
    subprocess.run(['./make_dir.sh', directory], capture_output=True, text=True, check=True)
    return directory

def concatenate_results(list_directories):
    i = 1
    for directory in list_directories:
        subprocess.run(["pdflatex", "-output-directory=" + directory , directory + '/graphics.tex'], 
                       capture_output=True, text=True, check=True)
        subprocess.run(["pdflatex", "-output-directory=" + directory , directory + '/graphics.tex'], 
                       capture_output=True, text=True, check=True)
        subprocess.run(["cp", directory + '/graphics.pdf', 'graphics/graphics_pdf/graphics'+str(i)+'.pdf'], 
                       capture_output=True, text=True, check=True)
        i = i + 1
        

def gen_couple_models_solvers(list_models_strings, list_solvers_strings):
    l_m = len(list_models_strings)
    l_s = len(list_solvers_strings)
    list_couple_models_solvers = []
    list_couple_strings = []
    for solver in list_solvers_strings:
        for i in range(l_m):
            for j in range(i+1,l_m):
                list_couple_models_solvers.append(({list_models_strings[i][0]:'black',
                                                    list_models_strings[j][0]:'blue'},
                                                    {solver[0]:'o'}))
                list_couple_strings.append(({list_models_strings[i][1]:'black',
                                             list_models_strings[j][1]:'blue'},
                                                   {solver[1]:'o'}))
    for model in list_models_strings:
        for i in range(l_s):
            for j in range(i+1,l_s):
                list_couple_models_solvers.append(({model[0]:'black'},{list_solvers_strings[i][0]:'o',
                                                                     list_solvers_strings[j][0]:'star'}))
                list_couple_strings.append(({model[1]:'black'},{list_solvers_strings[i][1]:'o',
                                                                     list_solvers_strings[j][1]:'star'}))
    return list_couple_models_solvers,list_couple_strings

def gen_table_proportion(data_name,df,list_frozen_time_min,models,solvers,models_string,
                         solvers_string,type_inst_res_list,num_inst,file,timeout):
    models['algo'] = 'brown'
    solvers['algo'] = 'oplus'
    models_string['algo'] = 'brown'
    solvers_string['algo'] = 'oplus'

    models['algo_enhanced'] = 'red'
    solvers['algo_enhanced'] = 'otimes'
    models_string['algo\\_enhanced'] = 'red'
    solvers_string['algo\\_enhanced'] = 'otimes'

    if luby :
        models['luby_test_full_restart'] = 'green'
        solvers['luby_test_full_restart'] = '+'
        models_string['luby\\_test\\_full\\_restart'] = 'green'
        solvers_string['luby\\_test\\_full\\_restart'] = '+'
        models['luby_test_medium_restart'] = 'blue'
        solvers['luby_test_medium_restart'] = 'o'
        models_string['luby\\_test\\_medium\\_restart'] = 'blue'
        solvers_string['luby\\_test\\_medium\\_restart'] = 'o'
        models['luby_test_small_restart']  = 'yellow'
        solvers['luby_test_small_restart'] = '-'
        models_string['luby\\_test\\_small\\_restart'] = 'yellow'
        solvers_string['luby\\_test\\_small\\_restart'] = '-'
    
        
    list_models = dict_keys_in_list(models)
    list_solvers = dict_keys_in_list(solvers)
    list_models_string = dict_keys_in_list(models_string)
    list_solvers_string = dict_keys_in_list(solvers_string)
    
    if data_name == 'netlib':
       data_name_tab = '\\texttt{Netlib}'
    if data_name == 'linear':
       data_name_tab = 'linear fitting'
    if data_name == 'polynomial':
       data_name_tab = 'polynomial fitting'
    if data_name == 'knapsack':
       data_name_tab = 'knapsack'
   
    #l = len(list_models_solver)
    maximum_allowed_time_in_hours = list_frozen_time_min[-1]//60
    file.write('\\begin{table}[]\\scriptsize\\centering\n\\caption{Results for '+data_name_tab+'-based instances at different time interval; the maximum allowed time is '+str(maximum_allowed_time_in_hours)+' hours}\n')
    file.write('\\begin{tabular}{@{}ll rr rr rr@{}}\\toprule Type of instances & Solving technique  & \\multicolumn{6}{c}{Instances solved (\\%, '+ str(num_inst) +' total) before threshold (minutes)}\\\\')
    file.write('\\cmidrule(l){3-8}&  &' + write_frozen_time(list_frozen_time_min) +' Timed out\\\\\n\\midrule\n')
    file.write('\\multirow{10}{*}{\\textsc{Equiv+Permut}} ')
    type_inst, results_test = type_inst_res_list[0]
    list_models_solver = write_model_solver(type_inst,list_models,list_solvers,list_models_string,list_solvers_string)
    models_solvers = evaluate_proportion(df,list_frozen_time_min,type_inst,num_inst,results_test,list_models_solver,timeout)
    i = 0
    for (model_solver,model,solver) in list_models_solver:
        proportions,time_out_percent = write_proportion(models_solvers[model_solver])
        i = 1
        file.write('& ' + model_solver + '  '+proportions+' & '+time_out_percent+' \\\\\n')
    type_inst, results_test = type_inst_res_list[1]
    file.write('\\midrule\\multirow{10}{*}{\\textsc{Non-Equiv+Permut}} ')
    list_models_solver = write_model_solver(type_inst,list_models,list_solvers,list_models_string,list_solvers_string)
    models_solvers = evaluate_proportion(df,list_frozen_time_min,type_inst,num_inst,results_test,list_models_solver,timeout)
    i = 0
    for (model_solver,model,solver) in list_models_solver:
        proportions,time_out_percent = write_proportion(models_solvers[model_solver])
        i = 1
        file.write('& ' + model_solver + '  '+proportions+' & '+time_out_percent+' \\\\\n')
    file.write('\\bottomrule\\end{tabular} \n\n')
    file.write('\\end{table} \n\n')

def gen_list_models_solvers_strings(list_models,list_models_str,list_solvers,list_solvers_str):
    list_models_strings = []
    list_solvers_strings = []
    l_m = len(list_models)
    l_s = len(list_solvers)
    if luby :
        algos = ['algo','algo\\_enhanced', 'luby\\_test\\_small\\_restart','luby\\_test\\_full\\_medium\\_restart']
    else :
        algos = ['algo','algo\\_enhanced']
    for i in range(l_m):
        if list_models_str[i] not in algos:
            list_models_strings.append((list_models[i],list_models_str[i]))
    for i in range(l_s):
        if list_solvers_str[i] not in algos:
            list_solvers_strings.append((list_solvers[i],list_solvers_str[i]))
    return list_models_strings, list_solvers_strings


def gen_graph_and_prop(data_name,directory,list_models,list_models_str,list_solvers,
                       list_solvers_str,num_clusters,num_inst,list_frozen_time_min):
    
    list_models_strings, list_solvers_strings = gen_list_models_solvers_strings(list_models,list_models_str,
                                                                                list_solvers,list_solvers_str)
    list_couple_models_solvers,list_couple_strings = gen_couple_models_solvers(list_models_strings, 
                                                                          list_solvers_strings)
    with open(directory+"/graphics.tex", "w") as file: 
        file.write(preambule1)
        file.write(preambule2)
        file.write(preambule3)
        file.write(preambule4)
        ith_grahic = 1
        for i in range(len(list_couple_models_solvers)):
            models, solvers = list_couple_models_solvers[i]
            models_string, solvers_string = list_couple_strings[i]
            for i in range(num_clusters):
                #directory = make_dir_for_two_models(models,solvers,i+1)
                #list_directories.append(directory)
                df,num_insts = data_clustered_by_size_num(i+1,num_clusters, data_name,set_web, set_inst,timeout)
                #print("df = ", df)
                instance_difficulty = type_cluster(i, num_insts,data_name)
                gen_graph_proportion(df,instance_difficulty,models,solvers,models_string,solvers_string,num_inst,
                                    file,ith_grahic,list_frozen_time_min)
                ith_grahic = ith_grahic + 1
        #concatenate_results(list_directories)
        file.write("\\end{document}")

def select_inst_by_size(df,min_size,max_size) :

    dataframe = df[(df['size'] >= min_size) & (df['size'] <= max_size)]

    return dataframe


list_frozen_time_min = [1,10,30,150,2880]
timeout = list_frozen_time_min[-1]

models = {'quadratic_model':'blue','theorem_model':'teal'}
solvers = {'coinbc':'o','cp-sat':'star','chuffed':'triangle','gecode':'*'}

models_string = {'quadratic\\_model':'blue','theorem\\_model':'teal'}
solvers_string = {'coinbc':'o','cp-sat':'star','chuffed':'triangle','gecode':'*'}

num_clusters = 1

data_name = 'netlib'
set_web = 'netlib/'
set_inst = 'lp'
#############################################
#data_name = 'knapsack'
#set_web = 'fractional_multidim_knapsack/'
#set_inst = 'knapsack'
#############################################
#data_name = 'polynomial'
#set_web = 'polynomial_fitting/'
#set_inst = 'polynomial'
#############################################
#data_name = 'linear'
#set_web = 'linear_fitting/'
#set_inst = 'linear'

table = 'graphics/table_percent_'+data_name+'/table.tex'

type_inst_res_list = [('equal by permutation','equivalent'),
                      ('not equal by permutation','not equivalent')]

################# all instances
min_size = 0
max_size = 148521033

################# medium and larges size instances
#min_size = 156652
#max_size = 148521033

################# small size instances
#min_size = 0
#max_size = 156651

with open(table, "w") as file:
    file.write(preambule1)
    file.write(preambule2)
    file.write(preambule3)
    file.write(preambule4)
    for i in range(num_clusters):
        df,total_num_insts = data_clustered_by_size_num(i+1,num_clusters, data_name,set_web, set_inst,timeout)
        print('total size of data = ', len(df))
        df = select_inst_by_size(df,min_size,max_size)
        print('size of selected data = ', len(df))
        #print(df['instance name'].drop_duplicates())
        num_inst = get_num_inst(df)
        print("num_inst for selected data = ", num_inst)
        #instance_difficulty = type_cluster(i, num_insts,data_name)
        gen_table_proportion(data_name,df,list_frozen_time_min,models,solvers,models_string,
                         solvers_string,type_inst_res_list,num_inst,file,timeout)
        
        file.write('\n\n\\newpage\n\n')

    file.write('\\end{document}')
    
    