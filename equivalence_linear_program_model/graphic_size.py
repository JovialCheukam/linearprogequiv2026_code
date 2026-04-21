import pandas as pd

luby = True

def gen_metadata(data,set_web,set_inst):

    Directory = "/Users/admin/Documents/POSTDOC_MichealMorin/CODE_PostDoc/env_code_postdoc_jovial/for_fir/code_exp_postdoc_jov_/results_exp/"
    if luby :
        df = pd.read_csv(Directory+"results_of_exp_luby_" + data + ".csv")
    else :
        df = pd.read_csv(Directory+"results_of_experiments_" + data + ".csv")

    df_meta = df[['instance name','number of variables','number of constraints','size']].drop_duplicates().sort_values(by='size').reset_index(drop=True)
    for i in range(len(df_meta)) :
        if df_meta.loc[i,'instance name'] == 'netlib/' :
            df_meta = df_meta.drop(labels=i, axis=0)
    df_meta.to_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')

    return df_meta

#data_name = 'knapsack'
#set_web = 'fractional_multidim_knapsack/'
#set_inst = 'knapsack'
#############################################
#data_name = 'linear'
#set_web = 'linear_fitting/'
#set_inst = 'linear'
#############################################
data_name = 'netlib'
set_web = 'netlib/'
set_inst = 'lp'
#############################################
#data_name = 'polynomial'
#set_web = 'polynomial_fitting/'
#set_inst = 'polynomial'

d = gen_metadata(data_name,set_web,set_inst)
duplicates = d[d.duplicated(subset=['instance name'],keep=False)]
print(duplicates)

# This routine converts seconds to a string in HH:MM:SS format.
def convert_secs_to_hour_min_sec(seconds):
    hours, remainder = divmod(seconds, 3600)
    hours = seconds // 3600
    remainder = seconds - hours * 3600
    minutes = remainder // 60
    seconds_ = remainder - minutes * 60
    return "{:02}:{:02}:{:02}".format(hours, minutes, seconds_)

def compute_time(time_string):
    format_seconds = time_string.split(':')
    time_second = float(format_seconds[0])*3600 + float(format_seconds[1])*60 + float(format_seconds[2])
    return time_second//60

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
            if df.loc[i,'type of instance'] == 'eq':
               df.loc[i,'type of instance'] = 'equal by permutation'
            if df.loc[i,'type of instance'] == 'not_eq':
               df.loc[i,'type of instance'] = 'not equal by permutation'
            if df.loc[i,"result of the test"] == 'not-equivalent':
               df.loc[i,"result of the test"] = 'not equivalent'
           
        i = i + 1
    j = 0
    for result in df["result of the test"]:
        if result in ["unknown due to timeout","unknown",'error:-should-be-not-equivalent','error:-should-be-equivalent']:
            df.loc[j,"execution time"] = convert_secs_to_hour_min_sec(timeout_min*60)
        if result in ['equivalent','not equivalent']:
            time_string = df.loc[j,"execution time"] 
            if compute_time(time_string) >= timeout_min :
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

def get_list_of_sizes_per_cluster(ith,num_cluster,data_name,set_web, set_inst,timeout_min):
    data = data_clustered_by_size_num(ith,num_cluster,data_name,set_web, set_inst,timeout_min)[0]
    l = len(data)
    list_sizes = []
    for i in range(l):
        if data.loc[i,'size'] not in list_sizes:
            list_sizes.append(data.loc[i,'size'])
    l_s = len(list_sizes)
    meta_data = pd.read_csv('data_set/'+set_web+'metadata_'+set_inst+'_set.csv')
    for i in range(l_s):
        df = meta_data[meta_data['size']==list_sizes[i]].reset_index()
        list_sizes[i] = (list_sizes[i],len(df),df.loc[0,'number of variables'],df.loc[0,'number of constraints'])
        
    return list_sizes
        

#df,size = data_clustered_by_size_num(1,1,data,set_web, set_inst,timeout_min)
#get_list_of_sizes_per_cluster(1,3)

preambule1 = "\\documentclass{book}\n\\usepackage[landscape,a3paper,left=0.5cm,right=1in,top=1in,"
preambule2 = "bottom=1in]{geometry}\n\\usepackage{amsmath}\n\\usepackage{xcolor}\n\\usepackage{tikz}\n"
preambule3 = "\\usepackage{pgfplots}\n\\usepackage{booktabs}\n\\usepackage{multirow}\n\\usepackage{amsmath,"
preambule4 = "amssymb}\n\n\\begin{document}\n\n\\vspace{2cm}\n\n"
preambule5 = "\\begin{figure}\\pgfplotsset{small,samples=10}\n\\centering\n\\begin{tabular}{lr}"


def end_graphic():
    return  "\n\\end{tabular}\\\\\n\\ref{storedcolorbar}\\end{figure}\n\n"


def write_head_env_graphi(type_axis,first_grahic,k):
    xlabel = 'number of variables'
    ylabel = 'number of constraints'
    if first_grahic:
        color_bar = "colorbar, colorbar horizontal,colorbar style={ylabel={Time (min)}, ylabel style={rotate=270},}, colorbar to name={storedcolorbar"+str(k)+"},"
    else:
        color_bar = ""
    return "\\begin{tikzpicture}[scale=0.6]\n\\begin{"+type_axis+"}[scatter, point meta=\\thisrow{time}, only marks, "+color_bar+", xlabel=" + xlabel +",ylabel=" + ylabel+"]\n"
    
def add_plot(color,mark,coordinates):
    return "\\addplot table[x=x,y=y]  { \n x y time\n "+ coordinates + "};\n"   

def end_env_graphic(type_axis):
    return "\\end{"+type_axis+"}\n\\end{tikzpicture}\n"


"""models = {'quadratic_model':'blue','theorem_model':'teal','algo':'brown','algo_enhanced':'red'}
solvers = {'gecode':'+','coinbc':'o','cp-sat':'star','chuffed':'triangle','algo':'oplus','algo_enhanced':'otimes'}"""



def matrix_graphics(data_name,type_inst_list,num_cluster,models,solvers,models_string,solvers_string,type_axis,directory,timeout_min):
    with open(directory+"/graphics.tex", "w") as file: 
        file.write(preambule1)
        file.write(preambule2)
        file.write(preambule3)
        file.write(preambule4)
        k = 1
        
        for i in range(num_cluster):
            ith_cluster = i + 1
            df, num_insts = data_clustered_by_size_num(ith_cluster,num_cluster,data_name,set_web, set_inst,timeout_min)
            for type_inst in type_inst_list:
                title_grahic = title_(type_inst)+ ' for ' + type_cluster(ith_cluster-1,num_insts,data_name)
                file.write('\\begin{figure}\\caption{' + title_grahic + '}\n\\vspace{0.5cm}\\centering\n\n')
                list_sizes = get_list_of_sizes_per_cluster(ith_cluster,num_cluster,data_name,set_web, set_inst,timeout_min) 
                
                table_graphics(df,list_sizes,type_inst,models,solvers,models_string,solvers_string,type_axis,file,k,timeout_min)
                file.write('\\end{figure}\n\n\\newpage\n\n')
                k = k + 1
        #gen_table_times_execution(df,list_sizes,type_inst,models,solvers,models_string,solvers_string,type_axis,file)
        file.write("\\end{document}")

def get_num_inst(df):
    l = len(df)
    inst = df.loc[0,'instance name']
    k = 1
    for i in range(l):
        if df.loc[i,'instance name'] != inst:
            k = k + 1
            inst = df.loc[i,'instance name']
    return k

def compute_times_execution(df,list_sizes,type_inst,solver,model,timeout_min):
    
    df_size = df[(df['solver'] == solver) & (df['type of instance'] == type_inst) & (df['model'] == model)]
    
    
    if type_inst in ['equal by permutation']:
        results_test = 'equivalent'
    else:
        results_test = 'not equivalent'
    df_size_solved = df[(df['solver'] == solver) & (df['type of instance'] == type_inst) & (df['result of the test'] == results_test) & (df['model'] == model)]
     
    res_solver_model_per_size = []
    if len(df_size)== 0:
        return res_solver_model_per_size
    
    for (size, num_insts, num_vars, num_ctrs) in list_sizes:
        df_ = df_size[df_size['size']==size].reset_index()
        total_time = 0
        if len(df_) == 0:
            total_time = timeout_min
        else:
            for i in range(len(df_)):
                total_time = total_time + compute_time(df_.loc[i,'execution time'])
        average_time_per_inst = round(total_time / num_insts,2)
        num_solv_inst = len(df_size_solved[df_size_solved['size']==size])
        res_solver_model_per_size.append((size, num_insts, num_vars, num_ctrs, total_time, 
                                         average_time_per_inst,num_solv_inst))
        
    return res_solver_model_per_size
    
    
def graph_coordinates_times_execution(df,list_sizes,type_inst,solver,model,color,mark,file,timeout_min):
    
    res_solver_model_per_size = compute_times_execution(df,list_sizes,type_inst,solver,model,timeout_min)
    #print("res_solver_model_per_size = ",res_solver_model_per_size)
    if res_solver_model_per_size == []:
        return ' '
    origin = res_solver_model_per_size[0]
    coordinates = str(origin[2]) + " " + str(origin[3]) + " 0.05 \n"
    for (size, num_insts, num_vars, num_ctrs, total_time, 
             average_time_per_inst,num_solv_inst) in res_solver_model_per_size:
            coordinates = coordinates + " " + str(num_vars) + " " + str(num_ctrs) + " " + str(average_time_per_inst) +" \n"
    coordonate = add_plot(color,mark,coordinates)
    return coordonate

def write_one_graphic(df,list_sizes,type_inst,model,color,solver,mark,
                                                 type_axis,file,first_grahic,k,timeout_min):
    coordonate = graph_coordinates_times_execution(df,list_sizes,type_inst,solver,model,color,mark,file,timeout_min)
    if coordonate == ' ':
        invalide = '\\begin{tabular}{c} Failing of the test execution \\\\ on Calcul Canada or \\\\ Invalid test \\\\\\end{tabular} '
       
        file.write("\\tikz{\\node (a) at (0.0,3.5) {"+invalide+"};\\node (a) at (0.0,0) {};}")
    else:
        file.write(write_head_env_graphi(type_axis,first_grahic,k)+'\n')
        file.write(coordonate)
        file.write('\n'+end_env_graphic(type_axis)+'\n')


def dict_keys_in_list(dictionary):
    list_keys = []
    for key in dictionary.keys():
        list_keys.append((key,dictionary[key]))
    return list_keys

def write_esperluette(i, model):
    if i == 0 :
        return "\\tikz{\\node (a) at (0.0,2.5) {"+model+"};\\node (a) at (0.0,0) {};} & "
    else:
        return ' & '

def write_backslash(i,c):
    if i == c-1:
        return "\\\\\n"
    else:
        return ""
    
def write_body_table(df,list_sizes,type_inst,r,c,models,solvers,models_string,solvers_string,type_axis,file,k,timeout_min):
    list_models = dict_keys_in_list(models)
    list_solvers = dict_keys_in_list(solvers)
    list_models_string = dict_keys_in_list(models_string)
    list_solvers_string = dict_keys_in_list(solvers_string)
    first_grahic = True
    for i in range(r):
        for j in range(c):
            file.write(write_esperluette(j,list_models_string[i][0])) 
            write_one_graphic(df,list_sizes,type_inst,list_models[i][0],list_models[i][1],
                                                     list_solvers[j][0],list_solvers[j][1],
                                                     type_axis,
                                                     file,first_grahic,k,timeout_min)
            file.write(write_backslash(j,c))
            first_grahic = False

def write_cols(c,bar):
    col = ""
    for _ in range(c):
        col = col + bar + 'c' 
    return col + bar

def write_solvers(solvers_string):
    list_solvers = dict_keys_in_list(solvers_string)
    solvs = " & "
    for solver in list_solvers[:-1]:
        solvs = solvs + solver[0] + " & "
    solvs = solvs + list_solvers[-1][0] + '\\\\\\\\'
    return solvs
            
def table_graphics(df,list_sizes,type_inst,models,solvers,models_string,solvers_string,type_axis, file, k,timeout_min):
    r = len(models)
    c = len(solvers)
    file.write('\\begin{tabular}{'+write_cols(c+1,"")+'}')
    file.write(write_solvers(solvers_string))
    write_body_table(df,list_sizes,type_inst,r,c,models,solvers,models_string,solvers_string,type_axis,file,k,timeout_min)
    file.write('\\\\\\\\\n\\multicolumn{'+str(c+1)+'}{c}{\\ref{storedcolorbar'+str(k)+'}}\\end{tabular}')


def write_models_solvers_row_res(l_s,l_m,size,list_models,list_solvers, dic_res_solver_model_per_size):
    models_solvers_row_T = ""
    models_solvers_row_A = ""
    models_solvers_row_N = ""
    for i in range(l_m):
        model = list_models[i][0]
        for j in range(l_s):
            solver = list_solvers[j][0]
            models_solvers_row_T = models_solvers_row_T + ' & ' + str(dic_res_solver_model_per_size[(size,model,solver)][4])
            models_solvers_row_A = models_solvers_row_A + ' & ' + str(dic_res_solver_model_per_size[(size,model,solver)][5])
            models_solvers_row_N = models_solvers_row_N + ' & ' + str(dic_res_solver_model_per_size[(size,model,solver)][6])
    row = '&  T.T. ' + models_solvers_row_T + '\\\\ & & A.T.P.I. ' + models_solvers_row_A + '\\\\ & & N.S.I. ' + models_solvers_row_N
    return row + '\\\\\\midrule\n'

def write_size_num_inst_title_time(df,list_sizes,type_inst,list_models,l_m,list_solvers,l_s,timeout_min):
    dic_res_solver_model_per_size = {}
    list_rows = ""
    l_s = len(list_solvers)
    l_m = len(list_models)
    for i in range(l_m):
        model = list_models[i][0]
        for j in range(l_s):
            solver = list_solvers[j][0]
            res_solver_model_per_size = compute_times_execution(df,list_sizes,type_inst,solver,model,timeout_min)
            k = 0
            for (size, num_insts, num_vars, num_ctrs) in list_sizes:
                dic_res_solver_model_per_size[(size,model,solver)] = res_solver_model_per_size[k]
                k = k + 1
    for (size, num_insts, num_vars, num_ctrs) in list_sizes:
        row1 = '\\multirow{3}{*}{$' + str(num_vars) + ' \\times ' + str(num_ctrs) 
        row2 = '$} & \\multirow{3}{*}{' + str(num_insts) +'}'  
        list_rows = list_rows + row1 + row2 + write_models_solvers_row_res(l_s,l_m,size,
                                                                           list_models,list_solvers,
                                                                           dic_res_solver_model_per_size)
        
    return list_rows
    
def write_models_solvers_row(list_models_string,l_s,list_solvers_string,l_m):
    models_solvers_row = ""
    for i in range(l_m):
        model = list_models_string[i][0]
        for j in range(l_s):
            solver = list_solvers_string[j][0]
            models_solvers_row = models_solvers_row + ' & ' + model + ' + ' + solver
    return models_solvers_row


def gen_table_times_execution(df,list_sizes,type_inst,models,solvers,models_string,solvers_string,type_axis, file):
    list_models = dict_keys_in_list(models)
    list_solvers = dict_keys_in_list(solvers)
    list_models_string = dict_keys_in_list(models_string)
    list_solvers_string = dict_keys_in_list(solvers_string)
    l_s = len(list_solvers)
    l_m = len(list_models)
    file.write('\\begin{tabular}{'+write_cols(3+l_s*l_m, "|")+'} \\toprule\n')
    file.write('Sizes & Num. of instances  &  ' + write_models_solvers_row(list_models_string,l_s,
                                                                        list_solvers_string,l_m) + '\\\\\\midrule\n')
    
    file.write(write_size_num_inst_title_time(df,list_sizes,type_inst,list_models,l_m,list_solvers,l_s))
    file.write('\\bottomrule\n\\end{tabular}')
    

def title_(type_inst):
    if type_inst == 'not equal by permutation':
        return '\\textsc{Non-Equiv + Permut}'
    else:
        return '\\textsc{Equiv + Permut}'

def underline(title_grahic):
    line = ""
    for _ in range(len(title_grahic)):
        line = line + '_'


timeout_min = 2880
type_inst_res_list = [('not equal by permutation','not equivalent')]
type_inst_list = ['equal by permutation','not equal by permutation']
type_axis = 'axis'
num_cluster = 1
#data_name = 'polynomial'
#data_name = 'linear'
#data_name = 'knapsack'
data_name = 'netlib'


########################################################################################## Different Graphics

models = {'quadratic_model':'blue','theorem_model':'teal'}
solvers = {'gecode':'+','coinbc':'o'}
models_string = {'\\textsc{QuadraMod}':'blue','\\textsc{TheoMod}':'teal'}
solvers_string = {'gecode':'+','coinbc':'o'}
directory = 'graphics/graphics_size_'+data_name+'_1'
matrix_graphics(data_name,type_inst_list,num_cluster,models,solvers,models_string,solvers_string,type_axis,directory,timeout_min)


########---------------------------------------------------------------------------------------

models = {'quadratic_model':'blue','theorem_model':'teal'}
solvers = {'cp-sat':'star','chuffed':'triangle'}
models_string = {'\\textsc{QuadraMod}':'blue','\\textsc{TheoMod}':'teal'}
solvers_string = {'cp-sat':'star','chuffed':'triangle'}
directory = 'graphics/graphics_size_'+data_name+'_2'
matrix_graphics(data_name,type_inst_list,num_cluster,models,solvers,models_string,solvers_string,type_axis,directory,timeout_min)

########---------------------------------------------------------------------------------------

models = {'algo':'brown','algo_enhanced':'red','luby_test_full_restart':'blue'}
solvers = {'no_solver':'oplus'}
models_string = {'\\textsc{Algo}':'brown','\\textsc{AlgoEnhanced}':'red','\\textsc{LubyTestFullRestart}':'blue'}
solvers_string = {'no-solver':'oplus'}
directory = 'graphics/graphics_size_'+data_name+'_3'
matrix_graphics(data_name,type_inst_list,num_cluster,models,solvers,models_string,solvers_string,type_axis,directory,timeout_min)