#include "VM.h"

VM::VM(){
#ifdef USE_GMP_LIB
  mpf_set_default_prec(1024);
#else
  BigNumber::begin(10);
#endif
  running = true;
  recsize = 0;
}

VM::~VM() {
#if THREADING == PROTOTHREADING
  while (threads.size() != 0) {
    for (int i = 0; i < threads.size(); i++) {
      threads[i].runNext();
      if (threads[i].isFinished()) {
        threads.erase(threads.begin() + i);
      }
    }
  }
#endif
}

void VM::attachMem(std::vector<Value> *mem){
  mempointer = mem;
}

std::vector<Value> VM::getStack(){
  return stack;
}

void VM::setStack(std::vector<Value> v){
  stack = v;
}

Value VM::disassemble(int prog, Value val){
  switch (prog) {
    case EXIT:    return "EXIT";
    case PUT:     return std::string(val.getType()? "PUT\tTXT" : "PUT\tNUM") + val.toString();
    case ADD:     return "ADD";
    case SUB:     return "SUB";
    case MUL:     return "MUL";
    case DIV:     return "DIV";
    case MOD:     return "MOD";
    case DLCALL:  return "DLCALL";
    case RUN:     return "RUN";
    case REC:     return "REC";
    case END:     return "END";
    case PRINT:   return "PRINT";
    case POP:     return "POP";
    case LOGSTCK: return "LOGSTCK";
    case PRINTLN: return "PRINTLN";
    case REPEAT:  return "REPEAT";
    case EQ:      return "EQ";
    case FEQ:     return "FEQ";
    case GT:      return "GT";
    case GE:      return "GE";
    case LT:      return "LT";
    case LE:      return "LE";
    case LAND:    return "LAND";
    case LOR:     return "LOR";
    case AND:     return "AND";
    case OR:      return "OR";
    case NOT:     return "NOT";
    case LNOT:    return "LNOT";
    case LSHIFT:  return "LSHIFT";
    case RSHIFT:  return "RSHIFT";
    case XOR:     return "XOR";
    case NEG:     return "NEG";
    case BREAK:   return "BREAK";
    case WTRUN:   return "WTRUN";
    case WFRUN:   return "WFRUN";
    case IFFRUN:  return "IFFRUN";
    case IFTRUN:  return "IFTRUN";
    case THREAD:  return "THREAD";
    case MEMGET:  return "MEMGET";
    case MEMSET:  return "MEMSET";
    case MEMPUT:  return "MEMPUT";
    case MEMSIZE: return "MEMSIZE";
    case MEMDEL:  return "MEMDEL";
    case MEMINS:  return "MEMINS";
    case TOTXT:   return "TOTXT";
    case TONUM:   return "TONUM";
    case ISNUM:   return "ISNUM";
    case CANNUM:  return "CANNUM";
    default:      return "???";
    return 0;
  }
}

void VM::printStack(){
  std::cout << "[";
  for(int i = 0; i < stack.size(); i++){
    std::cout << (stack[i].toString()) << ((i + 1) == stack.size()? "":", ");
  }
  std::cout << "]";
}

bool VM::run(std::vector<Value> prog, bool forceRun, int pc) {
  isBreaked = false;
  if(forceRun)running = true;
  for (; pc < prog.size(); pc++) {//fetch
    if(running)pc += run1(prog[pc].getLong(), (prog.size() - 1) == pc? 0:prog[pc + 1]);
    else break;
    if(isBreaked){
      isBreaked = false;
      return true;
    }
	}
  return false;
}

Value VM::pop(){
  Value v = stack[stack.size() - 1];
  stack.pop_back();
  return v;
}

Value VM::isGT(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getNumber() > v2.getNumber();
  }
  std::cerr << "STR in > ????";
  return 0;
}

Value VM::isGE(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getNumber() >= v2.getNumber();
  }
  std::cerr << "STR in >= ????";
  return 0;
}

Value VM::isLT(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getNumber() < v2.getNumber();
  }
  std::cerr << "STR in < ????";
  return 0;
}

Value VM::isLE(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getNumber() <= v2.getNumber();
  }
  std::cerr << "STR in <= ????";
  return 0;
}

Value VM::LAND2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getLong() && v2.getLong();
  } else {
    std::cerr << "STR in LOGICAL AND????" << std::endl;
  }
}

Value VM::LOR2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getLong() || v2.getLong();
  } else {
    std::cerr << "STR in LOGICAL OR????" << std::endl;
  }
}

Value VM::AND2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return (v1.getLong()) & (v2.getLong());
  } else {
    std::cerr << "STR in BITWISE AND????" << std::endl;
  }
}

Value VM::OR2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return (v1.getLong()) | (v2.getLong());
  } else {
    std::cerr << "STR in BITWISE OR????" << std::endl;
  }
}

Value VM::NOTval(Value v){
  if(!v.getType()){
    return ~(v.getLong());
  } else {
    std::cerr << "STR in BITWISE NOT????" << std::endl;
  }
}

Value VM::LNOTval(Value v){
  if(!v.getType()){
    return !v.getLong();
  } else {
    std::cerr << "STR in LOGICAL NOT????" << std::endl;
  }
}

Value VM::LSHIFT2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getLong() << v2.getLong();
  } else {
    std::cerr << "STR in LEFT SHIFT????" << std::endl;
  }
}

Value VM::RSHIFT2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return (v1.getLong()) >> (v2.getLong());
  } else {
    std::cerr << "STR in RIGHT SHIFT????" << std::endl;
  }
}

Value VM::XOR2val(Value v1, Value v2){
  if(!(v1.getType() && v2.getType())){
    return v1.getLong() ^ v2.getLong();
  } else {
    std::cerr << "STR in XOR????" << std::endl;
  }
}

Value VM::NEGval(Value v){
  if(!v.getType()){
#ifdef USE_GMP_LIB
    return mpf_class(NUMBER(-1) * v.getNumber());
#else
    return NUMBER(-1) * v.getNumber();
#endif
  } else {
    return v.reverse();
  }
}

std::vector<Value> VM::assemble(Value line){
  std::vector<Value> prog;
  line.trimLeft();
  if(line.startsWith("PUT") == 1) {
    line.substring(3);
    line.trimLeft();
    prog.push_back(PUT);
    if(line.startsWith("NUM") == 1) {
      line.substring(3);
      int i = 0;
      while (!(isdigit(line[i].toString()[0]) || line[i] == '.') || line[i] == '-') i++;
      line.substring(i);
      line.trim();
      line.toNum();
      prog.push_back(line);
    } else {
      line.substring(3);
      line.replace("\\n", "\n");
      line.replace("\\\n", "\\n");
      line.replace("\\t", "\t");
      line.replace("\\\t", "\\t");
      line.replace("\\r", "\r");
      line.replace("\\\r", "\\r");
      line.replace("\\a", "\a");
      line.replace("\\\a", "\\a");
      line.replace("\\b", "\b");
      line.replace("\\\b", "\\b");
      line.replace("\\f", "\f");
      line.replace("\\\f", "\\f");
      line.replace("\\\'", "\'");
      line.replace("\\\"", "\"");
      line.replace("\\\\", "\\");
      prog.push_back(line);
    }
  } else if(line.startsWith("ADD") == 1) {
    prog.push_back(ADD);
  } else if(line.startsWith("SUB") == 1) {
    prog.push_back(SUB);
  } else if(line.startsWith("MUL") == 1) {
    prog.push_back(MUL);
  } else if(line.startsWith("PRINTLN") == 1) {
    prog.push_back(PRINTLN);
  } else if(line.startsWith("DIV") == 1) {
    prog.push_back(DIV);
  } else if(line.startsWith("MOD") == 1) {
    prog.push_back(MOD);
  } else if(line.startsWith("PRINT") == 1) {
    prog.push_back(PRINT);
  } else if(line.startsWith("DLCALL") == 1) {
    prog.push_back(DLCALL);
  } else if(line.startsWith("REC") == 1) {
    prog.push_back(REC);
  } else if(line.startsWith("END") == 1) {
    prog.push_back(END);
  } else if(line.startsWith("RUN") == 1) {
    prog.push_back(RUN);
  } else if(line.startsWith("POP") == 1) {
    prog.push_back(POP);
  } else if(line.startsWith("LOGSTCK") == 1) {
    prog.push_back(LOGSTCK);
  } else if(line.startsWith("REPEAT") == 1) {
    prog.push_back(REPEAT);
  } else if(line.startsWith("EQ") == 1) {
    prog.push_back(EQ);
  } else if(line.startsWith("FEQ") == 1) {
    prog.push_back(FEQ);
  } else if(line.startsWith("GT") == 1) {
    prog.push_back(GT);
  } else if(line.startsWith("GE") == 1) {
    prog.push_back(GE);
  } else if(line.startsWith("LT") == 1) {
    prog.push_back(LT);
  } else if(line.startsWith("LE") == 1) {
    prog.push_back(LE);
  } else if(line.startsWith("LAND") == 1) {
    prog.push_back(LAND);
  } else if(line.startsWith("AND") == 1) {
    prog.push_back(AND);
  } else if(line.startsWith("LOR") == 1) {
    prog.push_back(LOR);
  } else if(line.startsWith("OR") == 1) {
    prog.push_back(OR);
  } else if(line.startsWith("NOT") == 1) {
    prog.push_back(NOT);
  } else if(line.startsWith("LNOT") == 1) {
    prog.push_back(LNOT);
  } else if(line.startsWith("LSHIFT") == 1) {
    prog.push_back(LSHIFT);
  } else if(line.startsWith("RSHIFT") == 1) {
    prog.push_back(RSHIFT);
  } else if(line.startsWith("XOR") == 1) {
    prog.push_back(XOR);
  } else if(line.startsWith("NEG") == 1) {
    prog.push_back(NEG);
  } else if(line.startsWith("BREAK") == 1) {
    prog.push_back(BREAK);
  } else if(line.startsWith("WTRUN") == 1) {
    prog.push_back(WTRUN);
  } else if(line.startsWith("WFRUN") == 1) {
    prog.push_back(WFRUN);
  } else if(line.startsWith("IFFRUN") == 1) {
    prog.push_back(IFFRUN);
  } else if(line.startsWith("IFTRUN") == 1) {
    prog.push_back(IFTRUN);
  } else if(line.startsWith("THREAD") == 1) {
    prog.push_back(THREAD);
  } else if(line.startsWith("MEMGET") == 1) {
    prog.push_back(MEMGET);
  } else if(line.startsWith("MEMSET") == 1) {
    prog.push_back(MEMSET);
  } else if(line.startsWith("MEMSIZE") == 1) {
    prog.push_back(MEMSIZE);
  } else if(line.startsWith("MEMPUT") == 1) {
    prog.push_back(MEMPUT);
  } else if(line.startsWith("MEMINS") == 1) {
    prog.push_back(MEMINS);
  } else if(line.startsWith("MEMDEL") == 1) {
    prog.push_back(MEMDEL);
  } else if(line.startsWith("TOTXT") == 1) {
    prog.push_back(TOTXT);
  } else if(line.startsWith("TONUM") == 1) {
    prog.push_back(TONUM);
  } else if(line.startsWith("CANNUM") == 1) {
    prog.push_back(CANNUM);
  } else if(line.startsWith("ISNUM") == 1) {
    prog.push_back(ISNUM);
  } else if(line.startsWith("EXIT") == 1) {
    prog.push_back(EXIT);
  }
  return prog;
}

std::vector<VM::Record> VM::mkRec(std::vector<Value> vals){
  std::vector<VM::Record> records;
  for(Value v : vals){
    if(v.getType() == TYPE_NUM){
      Record r;
      r.type = TYPE_NUM;
      r.value = v.getDouble();
      records.push_back(r);
    } else {
      Record r;
      std::string str = v.getString();
      r.type = TYPE_TEXT;
      for (char i : str) {
        r.value = i;
        records.push_back(r);
      }
    }
  }
  return records;
}


bool VM::run1(int prog, Value arg){
#if THREADING == PROTOTHREADING
  for (int i = 0; i < threads.size(); i++) {
    threads[i].runNext();
    if (threads[i].isFinished()) {
      threads.erase(threads.begin() + i);
    }
  }
#endif
  if(rec){
    if(prog == END){
      rec--;
      if(rec == 0){
        stack.push_back(recsize);
        recsize = 0;
      } else {
        stack.push_back(prog);
        recsize++;
      }
    } else {
      stack.push_back(prog);
      recsize++;
      if(prog == PUT){
        stack.push_back(arg);
        recsize++;
        return 1;
      } else if(prog == REC){
        rec++;
      }
    }
    return 0;
  }
#ifdef VM_DISASSEMBLE
  std::cout << disassemble(prog, arg);
#ifdef VM_STACKLOG
  std::cout << "\t\t\t";
#else
  std::cout << std::endl;
#endif
#endif
#ifdef VM_STACKLOG
  printStack();
  std::cout << std::endl;
#endif
  bool res = 0;
  switch (prog) {
    case EXIT:
      running = false;
      exit_code = pop().getLong();
      if(autoKill)exit(exit_code);
      break;
    case PUT:
      stack.push_back(arg);
      res = 1;
      break;
    case ADD:
      stack.push_back(pop() + pop());
      break;
    case SUB:
      stack.push_back(pop() + pop());
      break;
    case MUL:
      stack.push_back(pop() * pop());
      break;
    case DIV:
      stack.push_back(pop() / pop());
      break;
    case MOD:
      stack.push_back(pop() % pop());
      break;
    case PRINT:
      std::cout << pop().toString();
      break;
    case REC:
      rec++;
      break;
    case RUN: {
      std::vector<Value> prog;
      int ps = pop().getLong();
      for(; ps > 0; ps--){
        prog.insert(prog.begin(), pop());
      }
      run(prog);
      break;
    }
#ifndef DYNAMIC_LIBS_NOT_AVAILABLE
    case DLCALL: {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
      HINSTANCE hinstLib;
      dlfunc fn;
      std::string s = pop().getString();
      hinstLib = LoadLibraryA(s.c_str());
      if(hinstLib != NULL){
        fn = (dlfunc) GetProcAddress(hinstLib, (LPCSTR)pop().getString());
        if (NULL != fn){
          fn(this);
        } else {
          std::cerr << "SYMBOL NOT FOUND" << GetLastError();
        }
        FreeLibrary(hinstLib);
      } else {
        std::cerr << "CANNOT OPEN LIBRARY\n";
        std::cerr << GetLastError();
      }
#else
      void* lib;
      dlfunc fn;
      lib = dlopen(pop().toString().c_str(), RTLD_LAZY);
      if(!lib){
        std::cerr << "CANNOT OPEN LIBRARY\n";
        std::cerr << dlerror();
        break;
      }
      dlerror();//clear errors
      fn = ((dlfunc)dlsym(lib, pop().toString().c_str()));
      fn(this);
      dlclose(lib);
#endif
      break;
    }
#endif
    case POP:
      stack.pop_back();
      break;
    case LOGSTCK:
      printStack();
      break;
    case PRINTLN:
      std::cout << std::endl;
      break;
    case REPEAT: {
      int count = pop().getLong();
      std::vector<Value> prog;
      int ps = pop().getLong();
      for(; ps > 0; ps--){
        prog.insert(prog.begin(), pop());
      }
      for(; count > 0; count--){
        if (run(prog)) {
          break;
        }
      }
      break;
    }
    case EQ:
      stack.push_back(pop() == pop());
      break;
    case FEQ:
      stack.push_back(pop().strictEquals(pop()));
      break;
    case GT:
      stack.push_back(isGT(pop(), pop()));
      break;
    case GE:
      stack.push_back(isGE(pop(), pop()));
      break;
    case LT:
      stack.push_back(isLT(pop(), pop()));
      break;
    case LE:
      stack.push_back(isLE(pop(), pop()));
      break;
    case LAND:
      stack.push_back(LAND2val(pop(), pop()));
      break;
    case LOR:
      stack.push_back(LOR2val(pop(), pop()));
      break;
    case AND:
      stack.push_back(AND2val(pop(), pop()));
      break;
    case OR:
      stack.push_back(OR2val(pop(), pop()));
      break;
    case NOT:
      stack.push_back(NOTval(pop()));
      break;
    case LNOT:
      stack.push_back(LNOTval(pop()));
      break;
    case LSHIFT:
      stack.push_back(LSHIFT2val(pop(), pop()));
      break;
    case RSHIFT:
      stack.push_back(RSHIFT2val(pop(), pop()));
      break;
    case XOR:
      stack.push_back(XOR2val(pop(), pop()));
      break;
    case NEG:
      stack.push_back(NEGval(pop()));
      break;
    case BREAK:
      isBreaked = true;
      break;
    case WTRUN:
      if(stack.size() < 2)break;
      if(!stack[stack.size() - 1].getType()){
        bool tos = stack[stack.size() - 1].getLong();
        if(tos){
          stack.pop_back();
          std::vector<Value> prog;
          int ps = pop().getLong();
          for(; ps > 0; ps--){
            prog.insert(prog.begin(), pop());
          }
          while(tos){
            run(prog);
            tos = stack[stack.size() - 1].getLong();
            if(tos)stack.pop_back();
          }
        }
      }
      break;
    case WFRUN:
      if(stack.size() < 2)break;
      if(!stack[stack.size() - 1].getType()){
        bool tos = stack[stack.size() - 1].getLong();
        if(!tos){
          stack.pop_back();
          std::vector<Value> prog;
          int ps = pop().getLong();
          for(; ps > 0; ps--){
            prog.insert(prog.begin(), pop());
          }
          while(!tos){
            run(prog);
            tos = stack[stack.size() - 1].getLong();
            if(!tos)stack.pop_back();
          }
        }
      }
      break;
    case IFTRUN:
      if(stack.size() < 2)break;
      if(!stack[stack.size() - 1].getType()){
        bool tos = stack[stack.size() - 1].getLong();
        if(tos){
          stack.pop_back();
          std::vector<Value> prog;
          int ps = pop().getLong();
          for(; ps > 0; ps--){
            prog.insert(prog.begin(), pop());
          }
          isBreaked = run(prog);
        }
      }
      break;
    case IFFRUN:
      if(stack.size() < 2)break;
      if(!stack[stack.size() - 1].getType()){
        bool tos = stack[stack.size() - 1].getLong();
        if(!tos){
          stack.pop_back();
          std::vector<Value> prog;
          int ps = pop().getLong();
          for(; ps > 0; ps--){
            prog.insert(prog.begin(), pop());
          }
          isBreaked = run(prog);
        }
      }
      break;
    case THREAD: {
      std::vector<Value> prog;
      int ps = pop().getLong();
      for(; ps > 0; ps--){
        prog.insert(prog.begin(), pop());
      }
#if THREADING == PROTOTHREADING
      threads.push_back(Thread(prog, mempointer));
#else
      std::thread t([=]{
        VM *vm = new VM();
        vm->attachMem(mempointer);
        vm->run(prog);
      });
      t.detach();
#endif
      break;
    }
    case MEMSET: {
      int n = pop().getLong();
      if(mempointer->size() < (n + 1))mempointer->resize(n + 1);
      mempointer->at(n) = pop();
      break;
    }
    case MEMGET:
      stack.push_back(mempointer->at(pop().getLong()));
      break;
    case MEMSIZE:
      stack.push_back((int)mempointer->size());
      break;
    case MEMPUT:
      mempointer->push_back(pop());
      break;
    case MEMINS:
      mempointer->insert(mempointer->begin() + pop().getLong(), pop());
      break;
    case MEMDEL:
      mempointer->erase(mempointer->begin() + pop().getLong());
      break;
    case TOTXT:
      stack.push_back(pop().toString());
      break;
    case TONUM:
      stack[stack.size() - 1].toNum();
      break;
    case ISNUM:
      if(stack.size() == 0) break;
      stack.push_back(!stack[stack.size() - 1].getType());
      break;
    case CANNUM:
      if(stack.size() == 0) break;
      Value v = stack[stack.size() - 1];
      if(!v.getType()){
        stack.push_back(1);
      } else {
        int64_t i = 0;
        for(; v.getString()[i] != 0; i++){
          if(!isdigit(v.getString()[i])){
            stack.push_back(0);
            return res;
          }
        }
        stack.push_back(1);
      }
      break;
  }
  return res;
}
