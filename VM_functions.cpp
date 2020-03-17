#include "VM_functions.h"

value pop(){
  value v = stack[stack.size() - 1];
  stack.pop_back();
  return v;
}

void DLCALL_task(){
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  HINSTANCE hinstLib;
  dlfunc fn;
  std::string s = std::get<std::string>(pop());
  int len;
  int slength = (int)s.length() + 1;
  len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
  wchar_t* buf = new wchar_t[len];
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
  std::wstring r(buf);
  delete[] buf;
  hinstLib = LoadLibrary(r.c_str());
  if(hinstLib != NULL){
    fn = (dlfunc) GetProcAddress(hinstLib, (LPCSTR)std::get<std::string>(pop()).c_str());
    if (NULL != fn){
      stack = fn(stack);
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
  lib = dlopen(VM::val2str(pop()).c_str(), RTLD_LAZY);
  if(!lib){
    std::cerr << "CANNOT OPEN LIBRARY\n";
    std::cerr << dlerror();
    return;
  }
  dlerror();//clear errors
  fn = ((dlfunc)dlsym(lib, VM::val2str(pop()).c_str()));
  stack = fn(stack);
  dlclose(lib);
#endif
}

void RUN_task(){
  VM vm;
  std::vector<value> prog;
  int ps = std::get<double>(pop());
  for(int i = 0; i < ps; i++){
    prog.insert(prog.begin(), pop());
  }
  vm.setStack(stack);
  vm.autoKill = true;
  vm.run(prog);
  stack = vm.getStack();
}

void POP_task(){
  stack.pop_back();
}

void LOGSTCK_task(){
  std::cout << "[";
  for(int i = 0; i < stack.size(); i++){
    std::cout << VM::val2str(stack[i]) << ((i + 1) == stack.size()? "":", ");
  }
  std::cout << "]";
}

void PRINTLN_task(){
  std::cout << std::endl;
}

void PRINT_task(){
  std::cout << VM::val2str(pop());
}

void REPEAT_task(){
  VM vm;
  int count = VM::toNUM(pop());
  std::vector<value> prog;
  int ps = std::get<double>(pop());
  for(int i = 0; i < ps; i++){
    prog.insert(prog.begin(), pop());
  }
  vm.setStack(stack);
  for(; count > 0; count--){
    vm.autoKill = true;
    vm.run(prog);
    stack = vm.getStack();
  }
}

void WTRUN_task(){
  if(!VM::getValType(stack[stack.size() - 1])){
    bool tos = std::get<double>(stack[stack.size() - 1]);
    if(tos){
      stack.pop_back();
      std::vector<value> prog;
      int ps = std::get<double>(pop());
      for(; ps > 0; ps--){
        prog.insert(prog.begin(), pop());
      }
      while(tos){
        VM vm;
        vm.setStack(stack);
        vm.autoKill = true;
        vm.run(prog);
        stack = vm.getStack();
        tos = std::get<double>(stack[stack.size() - 1]);
        if(tos)stack.pop_back();
      }
    }
  }
}

void WFRUN_task(){
  if(!VM::getValType(stack[stack.size() - 1])){
    bool tos = std::get<double>(stack[stack.size() - 1]);
    if(!tos){
      stack.pop_back();
      std::vector<value> prog;
      int ps = std::get<double>(pop());
      for(; ps > 0; ps--){
        prog.insert(prog.begin(), pop());
      }
      while(!tos){
        VM vm;
        vm.setStack(stack);
        vm.autoKill = true;
        vm.run(prog);
        stack = vm.getStack();
        tos = std::get<double>(stack[stack.size() - 1]);
        if(tos)stack.pop_back();
      }
    }
  }
}

void EXIT_task(){
  exit(VM::toNUM(pop()));
}

void ADD_task(){
  stack.push_back(VM::add2val(pop(), pop()));
}

void SUB_task(){
  stack.push_back(VM::sub2val(pop(), pop()));
}

void MUL_task(){
  stack.push_back(VM::mul2val(pop(), pop()));
}

void DIV_task(){
  stack.push_back(VM::div2val(pop(), pop()));
}

void MOD_task(){
  stack.push_back(VM::mod2val(pop(), pop()));
}

void EQ_task(){
  stack.push_back(VM::isEQ(pop(), pop()));
}

void FEQ_task(){
  stack.push_back(VM::isFEQ(pop(), pop()));
}

void GT_task(){
  stack.push_back(VM::isGT(pop(), pop()));
}

void GE_task(){
  stack.push_back(VM::isGE(pop(), pop()));
}

void LT_task(){
  stack.push_back(VM::isLT(pop(), pop()));
}

void LE_task(){
  stack.push_back(VM::isLE(pop(), pop()));
}

void LAND_task(){
  stack.push_back(VM::LAND2val(pop(), pop()));
}

void AND_task(){
  stack.push_back(VM::AND2val(pop(), pop()));
}

void LOR_task(){
  stack.push_back(VM::LOR2val(pop(), pop()));
}

void OR_task(){
  stack.push_back(VM::OR2val(pop(), pop()));
}

void NOT_task(){
  stack.push_back(VM::NOTval(pop()));
}

void LNOT_task(){
  stack.push_back(VM::LNOTval(pop()));
}

void LSHIFT_task(){
  stack.push_back(VM::LSHIFT2val(pop(), pop()));
}

void RSHIFT_task(){
  stack.push_back(VM::RSHIFT2val(pop(), pop()));
}

void XOR_task(){
  stack.push_back(VM::XOR2val(pop(), pop()));
}

void NEG_task(){
  stack.push_back(VM::NEGval(pop()));
}

void PUT_task(int data){
  stack.push_back(data);
}

void PUT_task(const char* data){
  stack.push_back(std::string(data));
}
