
#include <stdio.h>
#include <wchar.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Map_answer.c"
#include "list_answer.c"
#include "treemap-answer.c"

typedef struct{
    char * nombreDocumento;
    int cantidadPalabras;
    int cantidadCaracteres;
}documento;

typedef struct{
    int cantidadLetras;
    char* nombrePalabra;
    int frecuencia;
    List* datosDoc;
    int cantidadDocs;
}palabra;

typedef struct{
    char * nombreDoc;
    int frecPalabraDoc;
}datosDoc;

#define MAX_ARGS 90
#define MAX_CADENA 100
#define MAXIMA_LONGITUD_CADENA 50

int isEqualString(void* key1, void*key2);
int lower_than_double(void * key1, void * key2);
int lower_than_string(void* key1, void* key2);
int extrae_argumentos(char *orig, char args[][MAX_CADENA], int max_args);
int contLetras(char args[]);
void minus(char* linea);
void* cargarDocumentos(Map* mapaGlobal, TreeMap* treemapDocs, int *numeroDocs);
void* documentoOrdenados(TreeMap* treemapDocs);
void* buscarPorPalabra(Map* mapaGlobal);
void* mostarPalabrasFrecuencia(Map* mapaGlobal, TreeMap * treemapDocs);
void* mostrarPalabrasRelevantes(Map* mapaGlobal, TreeMap * treemapDocs, int* numeroDocs);
void* buscarPalabraDoc(Map* mapaGlobal, TreeMap * treemapDocs);


int main(){
    
    Map* mapaGlobal = createMap(isEqualString);
    TreeMap * treemapDocs = createTreeMap(lower_than_string);
    int numeroDocs = 0;
    int opcion=1;

    while(opcion!=0){
        printf("============== DOCUMENTOS =============\n");
        printf("1. Cargar documentos.\n");
        printf("2. Mostrar documentos ordenados.\n");
        printf("3. Buscar por titulo.\n");
        printf("4. Palabras con mayor frecuencia.\n");
        printf("5. Palabras mas relevantes.\n");
        printf("6. Buscar palabra en documento.\n");
        printf("7. Mostrar palabra en su contexto del libro\n");
        printf("0. Salir.\n");
        scanf("%d", &opcion);

        switch(opcion){
            case 1:cargarDocumentos(mapaGlobal,treemapDocs,&numeroDocs);
            case 2:documentoOrdenados(treemapDocs);break;
            case 3:buscarPorPalabra(mapaGlobal);break;
            case 4:mostarPalabrasFrecuencia(mapaGlobal, treemapDocs);break;
            case 5:mostrarPalabrasRelevantes(mapaGlobal,treemapDocs,&numeroDocs);break;
            case 6:buscarPalabraDoc(mapaGlobal,treemapDocs);break;
        }
    }
    return 0;
}

/*
    Funcion para comparar claves de tipo string retorna 1 si son iguales
*/
int isEqualString(void * key1, void * key2){
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

/*
    Funcion para comparar claves de tipo int retorna 1 si son key1 > key2. ordena en orden descendente (mayor a menor)
*/
int lower_than_double(void * key1, void * key2) {
    if(*(double*)key1 > *(double*)key2) return 1;
    return 0;
}

/*
    Funcion para comparar claves de tipo string retorna 1 si son key1 < key2. ordena alfabeticamente 
*/
int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

/*
    Funcion que recive la cadena, ademas un arreglo que contenga las palabras y el numero maximo de palabras
    se retorna la cantidad de palabras que se encuentran en la cadena
*/
int extrae_argumentos(char *orig, char args[][MAX_CADENA], int max_args){
    
    char *tmp;
    int num=0;
    // guardamos memoria para la copia de la cadena 
    char *str = malloc(strlen(orig)+1);
    // copiamos la cadena que obtenermos con el fgets (linea original).
    strcpy(str, orig);
    // tokens: caracteres especiales que debe ignorar al momento de obtener la palabra 
    char tokens[] = " ',.;:-_!?¿¡(){}[]|@#~%&/\n\\\t";

    // con el tmp la primera palabra
    tmp=strtok(str, tokens);

    do{
        // verificamos si la canntidad de cadenas es mayor que la de palabras, si se cumple retornamos
        if (num == max_args){
            return (max_args+1);  
        }
        // El número de palabras máximo y salimos 
        strcpy(args[num], tmp); // copiamos la palabra en el arreglo
        num++;
        // extraemos la siguiente palabra
        tmp=strtok(NULL, tokens);
    }while (tmp!=NULL);// culmina cuando el tmp encuentre NULL

  return num;
}

/*
    Funcion que se le trasapasa la palabra y se calcula la cantidad de letras
*/
int contLetras(char args[]){
    const char* aux= (const char*)args;
    int cont = 0;
    // recorremos la palabra caracter por caracter
    for(int f = 0; f < strlen(aux); f++){
        cont++;
    }
    return cont; // retornamos la cantidad de letras que hay dentro de la palabra
}

/*
    Funcion que se les trasapasa la linea y se pasan a minuscula.
*/
void minus(char* linea){
    // utilizamos la funcion strlen(linea) para ir caracter por caracter usando la funcion
    // tolower 
    for(int i = 0; i < strlen(linea); i++){
        linea[i] = tolower(linea[i]);
    }
}

/*
    Funcion que recibe el nombre de un documento, verifica si ya esta ignorandolo, si no lo agrega a un Treemap para luego de ahi
    tomar cada palabra e ir agregandola a un Mapa 
*/
void* cargarDocumentos(Map *mapaGlobal, TreeMap * treemapDocs, int *numeroDocs){
    
    char archivo[1024];
     
    char *aux_nombreArchivo = (char*)malloc(sizeof(char));
    FILE * file;
    do{
        printf("Ingresar nombre archivo: \n");
        scanf("%s", &archivo);
        strcpy(aux_nombreArchivo, archivo);
        strcat(archivo, ".txt");
        file = fopen(archivo, "r");
    }while(!file);
    //Se hace el archivo

    //Se busca si el documento entregado ya existe en el treemap de docs.
    Pair * doc = searchTreeMap(treemapDocs,aux_nombreArchivo);

    bool valido = true;
    if(doc != NULL){ // Si no existe se devuelve al inicio de la funcion.
        printf("Documento ya esta cargado.\n");
        fclose(file);
        valido = false;
        cargarDocumentos(mapaGlobal,treemapDocs, numeroDocs);
    }else{
        (*numeroDocs)+=1;

        /////////// DATOS DEL DOCUMENTOS ///////////
        documento * nuevoDoc = (documento*)malloc(sizeof(documento));
        char * copia = (char *)malloc(50*sizeof(char));
        strcpy(copia,aux_nombreArchivo);
        nuevoDoc->nombreDocumento = copia;
        char ch;
        int caracteres=0, palabras=0;
        
        //Funcion que recorre todo el file contado las palabras y caracteres.
        while((ch = fgetc(file)) != EOF){
            caracteres++;
            if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\0'){
                palabras++;
            }
        }

        fclose(file);
        
        if(caracteres > 0){
            palabras++;
        }
        
        nuevoDoc -> cantidadPalabras = palabras;
        nuevoDoc -> cantidadCaracteres = caracteres;
        insertTreeMap(treemapDocs, nuevoDoc->nombreDocumento, nuevoDoc);

    }
    
    /////////// DATOS PALABRAS ///////////
    if(valido == true){
        char args[MAX_ARGS][MAX_CADENA];
        char lineaArchivo[1024];
        file = fopen(archivo, "r");

        while(fgets(lineaArchivo, 1024, file) != NULL){
            // verificar el salto de linea porque no nos deja sacar palabras despues de eso.
            minus(lineaArchivo);
            int nargs = extrae_argumentos(lineaArchivo,args,MAX_ARGS);
            int i;
              
            if(nargs > MAX_ARGS){
                printf ("Se han devuelto más palabras del máximo\n");
                nargs = MAX_ARGS;
            }
            printf("Cadena: %s\n", lineaArchivo);
            for(i = 0; i < nargs; i++){
                
                //Se busca si la palabra existe en el mapa
                palabra* nuevaPalabra = (palabra*) malloc (sizeof(palabra));
                char* auxNombre = (char*) malloc(sizeof(char));
                strcpy(auxNombre, args[i]);
                nuevaPalabra->nombrePalabra = auxNombre;
                
                Map * M = searchMap(mapaGlobal,nuevaPalabra->nombrePalabra);
                printf("BUSCANDO EN MAPA LA PALABRA: %s\n",nuevaPalabra->nombrePalabra);
                nuevaPalabra->frecuencia = 0;
                
                if(M==NULL){ //No existe palabra en el mapa.
                    
                    printf("Nueva palabra encontrada:     ");
                        
                    datosDoc * datosDocumento = (datosDoc*)malloc(sizeof(datosDoc));
                    List * L = createList();
                    datosDocumento->frecPalabraDoc=1;

                    char* copiaNombreArchivo = (char*) malloc(sizeof(char));
                    strcpy(copiaNombreArchivo,aux_nombreArchivo);
                    datosDocumento->nombreDoc = copiaNombreArchivo;

                    //Se meten los datos del documento en la lista.
                    pushFront(L,datosDocumento);

                    //Se rellena nuevaPalabra.
                    nuevaPalabra->datosDoc = L;
                    nuevaPalabra->cantidadDocs = 1;
                    nuevaPalabra->frecuencia++;
                    
                    //Se cuenta las letras con la funcion contLetras.
                    int cantidadDeLetras = contLetras(args[i]);  
                    nuevaPalabra->cantidadLetras = cantidadDeLetras;

                    //printf("Palabra: %s. Frecuencia: %d. Cantidad de letras: %d.\n", nuevaPalabra->nombrePalabra,nuevaPalabra->frecuencia,nuevaPalabra->cantidadLetras);
                    //printf("Palabra viene de documento nuevo ---- Nombre txt: %s. Frecuencia palabra en doc: %d.\n",datosDocumento->nombreDoc,datosDocumento->frecPalabraDoc);

                    //Se meten todos los datos en el mapa.
                    insertMap(mapaGlobal,nuevaPalabra->nombrePalabra,nuevaPalabra);
                        
                }
                else{ // EXISTE PALABRA EN MAPA

                    printf("Palabra ya existe en mapa:     ");
                    //Se crea una nueva variable que toma el valor de la palabra encontrada
                    palabra * palabraExiste = (palabra*)searchMap(mapaGlobal,nuevaPalabra->nombrePalabra);
                    palabraExiste->frecuencia++;
                        
                    //printf("Palabra: %s. Frecuencia: %d. Cantidad de letras: %d.\n", palabraExiste->nombrePalabra,palabraExiste->frecuencia,palabraExiste->cantidadLetras);

                    //Lista que toma el valor de datosDoc de la palabra encontrada.
                    List * list = createList();
                    list = palabraExiste->datosDoc;

                    datosDoc * datosDocumento = (datosDoc*)malloc(sizeof(datosDoc));
                    datosDocumento = firstList(palabraExiste->datosDoc);
                    bool existe = false;

                    char* copiaNombreTxt = (char*) malloc(sizeof(char));
                    strcpy(copiaNombreTxt,aux_nombreArchivo);

                    //Se busca si existe el documento en la lista que está en la palabra.
                    while(datosDocumento!=NULL){
                        if(strcmp(copiaNombreTxt,datosDocumento->nombreDoc) == 0){
                            datosDocumento->frecPalabraDoc++;
                            //printf("Palabra viene de documento EXISTENTE ---- Nombre txt: %s. Frecuencia palabra en doc: %d.\n",datosDocumento->nombreDoc,datosDocumento->frecPalabraDoc);
                            existe = true;
                            break;
                        }    
                        datosDocumento=nextList(palabraExiste->datosDoc);
                        if(palabraExiste->datosDoc == NULL)break;
                    }

                    // Nombre de doc no está en datosDoc de palabra, o sea el archivo de donde sale la palabra es nuevo.
                    if(existe == false){
                        palabraExiste->cantidadDocs += 1;

                        //Se crean los nuevos datos del documento y se meten en la lista.
                        datosDoc * datos = (datosDoc*)malloc(sizeof(datosDoc));
                        datos->frecPalabraDoc=1;
                        char* copiaTxt = (char*) malloc(sizeof(char));
                        strcpy(copiaTxt,aux_nombreArchivo);
                        datos->nombreDoc = copiaTxt;
                        //printf("Palabra viene de documento NUEVO ---- Nombre txt: %s. Frecuencia palabra en doc: %d.\n",datos->nombreDoc,datos->frecPalabraDoc);
                        pushFront(list,datos);
                    }
                }
                printf("\n\n");
            }
        }

        //Se verifica si se quiere importar otro txt.
        char * palabra = (char*)malloc(3*sizeof(char));
        char * confirmacion = (char*)malloc(3*sizeof(char));
        confirmacion = "SI";
                
        printf("Si desea cargar otro archivo escriba 'SI': ");
        scanf("%s", palabra);
        fclose(file);
        if(strcmp(palabra, confirmacion) == 0){//Si pone "SI", se vuelve al inicio de la funcion
            cargarDocumentos(mapaGlobal,treemapDocs, numeroDocs);
        }
        printf("Archivos cargados\n");
    }
}

/*
    Funcion que recorre el TreeMap de los documentos, mostrando cada uno de estos de forma ordenada
*/
void * documentoOrdenados(TreeMap * treemapDocs){
    
    Pair * docOrden = firstTreeMap(treemapDocs);

    while(docOrden != NULL){
        //Se crea un tipo documento para guardar lo que tiene en el pair.
        documento * docDatos = (documento*)malloc(sizeof(documento));
        docDatos = docOrden->value;
        printf("Nombre documento: %s \n", docOrden->key);
        printf("Numero palabras: %d \n", docDatos->cantidadPalabras);
        printf("Numero caracteres: %d \n", docDatos->cantidadCaracteres);
        
        docOrden = nextTreeMap(treemapDocs);
    }
    
}

/*
    Funcion para buscar palabra, en caso de que la palabra ingresada sea erronea o no exista se muestra por pantalla en caso contrario
    se espera que se encuentre la palabra deseada
*/
void * buscarPorPalabra(Map * mapaGlobal){

    char * palabraIngresada = (char*)malloc(50 * sizeof(char)); //  Modificacion cantidad 
    printf("Ingrese la palabra a buscar: ");
    scanf("%s", palabraIngresada); 
    
    // Se verifica si la palabra ingresada existe dentro de nuestro mapa global.
    
    const char * M = searchMap(mapaGlobal,palabraIngresada);

    if(M == NULL){ 
        printf("Palabra buscada no existe.\n");
    }
    else{
        printf("Palabra encontrada.\n");
        palabra * palabraExiste = (palabra*)searchMap(mapaGlobal,palabraIngresada);

        //Pasamos por la lista de los documento de la palabra para saber cuantos tiene.
        datosDoc * datosDocumento = (datosDoc*)malloc(sizeof(datosDoc));
        datosDocumento = firstList(palabraExiste->datosDoc);
        int cont = 0;
        while(datosDocumento != NULL){
            cont++;
            datosDocumento=nextList(palabraExiste->datosDoc);
        }
        
        int numeroDocs = cont;

        datosDocumento = firstList(palabraExiste->datosDoc);
        cont=0;

        //Se muestra la palabra con los datos del doc.
        while(numeroDocs != 0){
            cont++;
            if(cont = datosDocumento->frecPalabraDoc){
                printf("Documento: %s. Frecuencia palabra '%s': %d. Cantidad caracteres: %d.\n",datosDocumento->nombreDoc, palabraIngresada, datosDocumento->frecPalabraDoc,palabraExiste->cantidadLetras);
                numeroDocs--;
            }
            datosDocumento=nextList(palabraExiste->datosDoc);
            if(datosDocumento==NULL){
                datosDocumento = firstList(palabraExiste->datosDoc);
            }
            if(numeroDocs <= 0){
                break;
            }
        }
    }

}

/*
    Funcion que revisa en un documento, las frecuencias de sus palabras, para luego mostrar las 10 palabras (si es que la hay)
    con mayor frecuencia del documento
*/
void* mostarPalabrasFrecuencia(Map* mapaGlobal, TreeMap* treemapDocs){

    char* nombreTxt = (char*) malloc(50 * sizeof(char));
    printf("Ingrese el nombre del documento deseado: ");
    scanf("%s", nombreTxt);

    //Buscar si nombreDoc existe en listaDocs.
    // verificacion si existe el documento en nuestra lista
    bool valido = false;
    
    Pair* pair = searchTreeMap(treemapDocs,nombreTxt);

    if(pair != NULL){
        valido = true;
    }else{
        printf("Documento buscado no existe\n");
    }

    TreeMap * treeMapFrec = createTreeMap(lower_than_double);

    if(valido == true){
        documento* auxPair = (documento*)malloc(sizeof(malloc));
        auxPair = pair->value;

        FILE * txt;
        char * copyNombreTxt = (char*)malloc(50*sizeof(char));
        strcpy(copyNombreTxt,nombreTxt);
        strcat(nombreTxt,".txt");
        txt = fopen(nombreTxt, "r");

        // Crear lista con struct "relevantes" (RELEVANCIA Y nombrePalabraR)

        palabra * M;
        M = firstMap(mapaGlobal);

        while(M != NULL){

            // Recorrer mapaGlobal, solo nos importan las palabras que tengan el doc buscado asociado a su datosDoc.
            // Si encuentra una palabra que tiene el documento buscado:
            List * listAux = M->datosDoc;
            datosDoc * datosAux = firstList(listAux);

            while(datosAux != NULL){

                if(strcmp(datosAux->nombreDoc,copyNombreTxt) == 0){
                    
                    char * nombrePalF = (char*)malloc(50*sizeof(char));
                    strcpy(nombrePalF, (M->nombrePalabra));
                    
                    //Se realiza la formula para sacar la frecuencia.
                    double cosa;
                    cosa = ((double)datosAux->frecPalabraDoc)/((double)auxPair->cantidadPalabras);
                    double* frecuencia= (double*)malloc(sizeof(double));
                    *frecuencia = cosa;
                    printf("%s ",nombrePalF);
                    printf("%.2lf\n", *frecuencia);
                    
                    //Se crea una lista de palabras que contegan la misma frecuencia
                    Pair* buscarR = (Pair*)malloc(sizeof(Pair));
                    buscarR = searchTreeMap(treeMapFrec,frecuencia);

                    if(buscarR == NULL){
                        // No existe la frecuencia en el mapa
                        List* listaFR = createList();
                        pushBack(listaFR,nombrePalF);
                        insertTreeMap(treeMapFrec,frecuencia,listaFR);
                    }else{ //Frecuencia ya existe en el mapa
                        List* listaFr = buscarR->value;
                        pushBack(listaFr,nombrePalF);
                    }
                    
                }
                datosAux = nextList(listAux);
                if(datosAux == NULL)break;
            }

            M = nextMap(mapaGlobal);
            if(M == NULL)break;
        }

        //Se mustran las 10 palabras con mas frecuencias
        //Con el pair guardamos el value y key de la palabra.
        Pair * par = firstTreeMap(treeMapFrec);
        int contDiez = 0;
        printf("\n");
        while(par != NULL){
            List* lista = par->value;
            double* auxList = (double*)malloc(sizeof(double));
            auxList = firstList(lista);
            //Como el value es una lista, se recorre para mostrar las palabras dentro de esta.
            while(auxList != NULL){
                printf("Palabra: %s. Relevancia: %.2lf.\n", auxList, *(double*)par->key);//par->key, par->value);
                contDiez+=1;   
                if(contDiez == 10)break;
                auxList = nextList(lista);
                if(auxList == NULL)break;
            }
            par = nextTreeMap(treeMapFrec);
            if(contDiez == 10)break;
            if(par == NULL)break;
        }
        
    }
}

/*
    Funcion  que verifica si el documento ingreaso se encuentra en la lista docs. Dentro de este se crea una lista que guarda las palabras
    relevantes (se utiliza una formula para calcular la relevancia de las palabras), se ordenan por relevancia y se muestra por pantalla.
*/
void* mostrarPalabrasRelevantes(Map* mapaGlobal, TreeMap* treemapDocs, int* numeroDocs){

    char* nombreTxt = (char*) malloc(50 * sizeof(char));
    printf("Ingrese el nombre del documento deseado: ");
    scanf("%s", nombreTxt);
    
    //Buscar si nombreDoc existe en listaDocs.
    // verificacion si existe el documento en nuestra lista

    bool valido = false;

    Pair* pair = searchTreeMap(treemapDocs,nombreTxt);

    if(pair != NULL){
        valido = true;
    }else{
        printf("Documento buscado no existe\n");
    }

    TreeMap * treeMapPR = createTreeMap(lower_than_double);

    if(valido = true){
        documento* docAux = (documento*)malloc(sizeof(malloc));
        docAux = pair->value;

        FILE * txt;
        char * copyNombreTxt = (char*)malloc(50*sizeof(char));
        strcpy(copyNombreTxt,nombreTxt);
        strcat(nombreTxt,".txt");
        txt = fopen(nombreTxt, "r");
        palabra * M;
        M = firstMap(mapaGlobal);
        
        while(M != NULL){

            // Recorrer mapaGlobal, solo nos importan las palabras que tengan el doc buscado asociado a su datosDoc.
            // Si encuentra una palabra que tiene el documento buscado:
            List * listAux = M->datosDoc;
            datosDoc * datosAux = firstList(listAux);
            while(datosAux != NULL){

                if(strcmp(datosAux->nombreDoc,copyNombreTxt)==0){

                    char* auxNombrePalabra = (char*)malloc(50*sizeof(char));
                    strcpy(auxNombrePalabra, (M->nombrePalabra));

                    //Se realiza la formual de relevancia.
                    double primeraParte = ((double)(datosAux->frecPalabraDoc)/(docAux->cantidadPalabras));
                    double segundaParte = log(((double)*numeroDocs)/M->cantidadDocs);
                    double* palabraR = (double*)malloc(sizeof(double));

                    *palabraR = (primeraParte * segundaParte);

                    //Se busca en el treemap si ya existe la relevancia.
                    Pair* buscarR = (Pair*)malloc(sizeof(Pair));
                    buscarR = searchTreeMap(treeMapPR,palabraR);
                    
                    if(buscarR == NULL){
                        // No existe la relevancia en el mapa
                        List* listaFR = createList();
                        pushBack(listaFR,auxNombrePalabra);
                        insertTreeMap(treeMapPR,palabraR,listaFR);
                    }else{ // Relevancia ya existe en el mapa
                        List* listaFr = buscarR->value;
                        pushBack(listaFr,auxNombrePalabra);
                    }
                }
                datosAux = nextList(listAux);
                if(datosAux == NULL){
                    break;
                }
            }

            M = nextMap(mapaGlobal);
            if(M == NULL)break;
        }

        //Se mustran las 10 palabras con mas relevancia
        //Con el pair guardamos el value y key de la palabra.
        Pair * par = firstTreeMap(treeMapPR);
        int contDiez = 0; 
        printf("\n");
        while(par != NULL){
            List* listaRelevantes = par->value;
            double* auxList = (double*)malloc(sizeof(double));
            auxList = firstList(listaRelevantes);
            //Como el value es una lista, se recorre para mostrar las palabras dentro de esta.
            while(auxList != NULL){
                printf("Palabra: %s. Relevancia: %.2lf.\n", auxList, *(double*)par->key);//par->key, par->value);
                contDiez+=1;   
                if(contDiez == 10)break;
                auxList = nextList(listaRelevantes);
                if(auxList == NULL)break;
            }
            par = nextTreeMap(treeMapPR);
            if(contDiez == 10)break;
            if(par == NULL)break;
        }

    }
}

/*
    Funcion que se que verifica si la palabra se encuentra en el documentos seleccionado, en el caso de encotrar el doc
    se busca la palabra y se prinftea: la palabra previa, la palabra buscada y la palabra siguiente
*/
void* buscarPalabraDoc(Map* mapaGlobal, TreeMap* treemapDocs){

    char* nombreDoc = (char*)malloc(50*sizeof(char));
    printf("Ingrese el nombre del documento: ");
    scanf("%s", nombreDoc);
    
    bool valido = false;
    Pair* pair = searchTreeMap(treemapDocs,nombreDoc);

    if(pair != NULL){
        valido = true;
    }else{
        printf("Documento buscado no existe.\n");
    }

    if(valido == true){
        printf("El documento fue encontrado.\n");

        char* nombrePalabra = (char*)malloc(100 * sizeof(char));
        printf("Ingrese el nombre de la palabra: ");
        scanf("%s", nombrePalabra);

        FILE * txt;
        strcat(nombreDoc,".txt");
        txt = fopen(nombreDoc, "r");

        char args[MAX_ARGS][MAX_CADENA];
        char lineaArchivo[1024];

        // obtenemos la linea del archivo
        while(fgets(lineaArchivo, 1024, txt) != NULL){
            // pasamos a minusucla la linea completa con al funcion "minus"
            minus(lineaArchivo);
            int nargs = extrae_argumentos(lineaArchivo,args,MAX_ARGS); // funcion que obtiene las palabras de la linea
            int i;
            // verificamos si la cantidad de palabras es mayor a lo establecido en el define
            if(nargs > MAX_ARGS){
                printf ("Se han devuelto más palabras del máximo\n");
                nargs = MAX_ARGS;
            }
            // nargs representa la cantidad de lineas que hay dentro de la liena y se usa para ir pasando por cada palabra
            for(i = 0; i < nargs; i++){
                // varible que contiene la palabra en la posicion args[i]
                char* aux = (char*) malloc(50*sizeof(char));
                strcpy(aux, args[i]);
                // comprobamos si existe la palabra en la linea del archivo

                if(strcmp(aux,nombrePalabra) == 0){

                    printf("Contexto palabra: ");
                    // args[i-1] corresponde a la palabra anterior a la buscada y el if es para comprobar si en esa posicion
                    // no existe nada entonces printfteara que no encontro nada
                    if(args[i-1] == NULL){ 
                        printf("no hay palabra previa a la actual\n");
                    }else printf("%s ", args[i-1]); // si hay palabra se imprime
                    // imprimimos la palabra actual
                    printf("%s ", args[i]);

                    // args[i+1] corresponde a la palabra posterior a la buscada y el if es para comprobar si en esa posicion
                    // no existe nada entonces printfteara que no encontro nada
                    if(args[i+1] == NULL){
                        printf("no hay palabra post a la actual\n");
                    }else printf("%s\n", args[i+1]); // si hay palabra se imprime
                }
            }       
        }

    }else printf("Doc no encontrado.\n");
}