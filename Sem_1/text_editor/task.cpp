#include "task.h"
#include "logged_command_wrapper.h"

/* Курсор влево */
class MoveCursorLeftCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            if(cursorPosition != 0){
                cursorPosition--;
                if(buffer[cursorPosition] == '\n'){
                    cursorPosition--;
                }
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitMoveCursorLeftCommand(*this);
        }
};

/* Курсор вправо */
class MoveCursorRightCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            if(cursorPosition != buffer.size()){
                cursorPosition++;
                if(buffer[cursorPosition] == '\n'){
                    cursorPosition++;
                }
            }
            editor.UnselectText();
        }
        
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitMoveCursorRightCommand(*this);
        }
};

/* Курсор вверх */
class MoveCursorUpCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            
            auto n_pos = buffer.rfind('\n', cursorPosition - 1);
            //string above exists
            if(n_pos != std::string::npos){
                //relative distance from currennt string begining
                size_t dst = cursorPosition - n_pos;
                
                auto new_n_pos = buffer.rfind('\n', n_pos - 1);
              
                //only above exists
                if(new_n_pos == std::string::npos){
                    //min if upper string is shorter than the current one
                    cursorPosition = std::min(dst - 1, n_pos - 1);
                //above - above string exists
                }else{
                    cursorPosition = std::min(new_n_pos + dst , n_pos - 1);
                }
            }//else{
                //size_t dst = cursorPosition;
            //}
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitMoveCursorUpCommand(*this);
        }
};

/* Курсор вниз */
class MoveCursorDownCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            auto n_pos = buffer.find('\n',cursorPosition + 1);
            //string below exists
            if(n_pos != std::string::npos){
                auto above_n_pos = buffer.rfind('\n',cursorPosition);
                int dst = cursorPosition;
                //above string exists
                if(above_n_pos != std::string::npos){
                    //dst = cursorPosition - above_n_pos;
                    dst -= above_n_pos;
                }//else{// current string is the first
                //    dst = cursorPosition;
                //}
                auto below_n_pos = buffer.find('\n',n_pos + 1);
                //below below string exists
                if(below_n_pos != std::string::npos){
                    cursorPosition = std::min(n_pos + dst, below_n_pos - 1);
                }else{//below string is the last one
                    cursorPosition = std::min(n_pos + dst, buffer.size());
                }
            }//else{
            //}
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitMoveCursorDownCommand(*this);
        }
};

/* Выделить текст */
class SelectTextCommand : public ICommand {
    int selec_size_;
    public:
        SelectTextCommand(int selec_size) : selec_size_(selec_size) {}
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            editor.SelectText(cursorPosition, cursorPosition + selec_size_);
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitSelectCommand(*this);
        }
};

/* Ввести текст */
class InsertTextCommand : public ICommand {
    std::string insert_text_;
    public:
        InsertTextCommand(std::string insert_text) : insert_text_(insert_text) {}
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            if(editor.HasSelection()){
                auto pa = editor.GetSelection();
                size_t cur_p = pa.first;
                buffer.erase(pa.first, pa.second - pa.first);
                buffer.insert(cur_p,insert_text_);
            }else{
                buffer.insert(cursorPosition, insert_text_);
            }
            cursorPosition += insert_text_.size();
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitInsertTextCommand(*this);
        }
};

/* Удалить текст */
class DeleteTextCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            if(editor.HasSelection()){
                auto pa = editor.GetSelection();
                buffer.erase(pa.first, pa.second - pa.first);
                cursorPosition = pa.first;
            }else{
                buffer.erase(cursorPosition, 1);
                cursorPosition--;
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitDeleteTextCommand(*this);
        }
};

/* Скопировать текст */
class CopyTextCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            if(editor.HasSelection()){
                auto pa = editor.GetSelection();
                clipboard = buffer.substr(pa.first, pa.second - pa.first);
            }else{
                clipboard = editor.GetCharUnderCursor();
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitCopyTextCommand(*this);
        }
};

/* Вставить скопированный текст */
class PasteTextCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            if(editor.HasSelection()){
                auto pa = editor.GetSelection();
                size_t cur_pos = pa.first;
                buffer.erase(pa.first,pa.second - pa.first);
                buffer.insert(cur_pos,clipboard);
            }else{
                buffer.insert(cursorPosition,clipboard);
            }
            cursorPosition += clipboard.size();
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitPasteTextCommand(*this);
        }
};

/* Привести выделенный текст в ВЕРХНИЙ регистр */
class UppercaseTextCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            auto pa = editor.GetSelection();
            for (size_t i = pa.first; i <= pa.second; ++i ){
                buffer[i] = toupper(buffer[i]);
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitUppercaseTextCommand(*this);
        }
};

/* Привести выделенный текст в нижний регистр */
class LowercaseTextCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            auto pa = editor.GetSelection();
            for (size_t i = pa.first; i <= pa.second; ++i ){
                buffer[i] = tolower(buffer[i]);
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitLowercaseTextCommand(*this);
        }
};

/* Перенести курсор в конец строки */
class MoveToEndCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            size_t pos = buffer.find('\n', cursorPosition);// +1?
            if(pos == std::string::npos){
                cursorPosition = buffer.size();
            }else{
                cursorPosition = pos ;
                editor.UnselectText();
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitMoveToEndCommand(*this);
        }
};

/* Перенести курсор в начало строки */
class MoveToStartCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            size_t pos = buffer.rfind('\n', cursorPosition);// -1?
            cursorPosition = 0;
            if(pos != std::string::npos){
                cursorPosition = pos + 1;
            }
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitMoveToStartCommand(*this);
        }
};

/* Удалить часть строки, начиная от позиции курсора до первого пробела или конца строки */
class DeleteWordCommand : public ICommand {
    public:
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            size_t pos = cursorPosition;
            for(; pos < buffer.size(); ++pos){
                if(buffer[pos] == ' ' || buffer[pos] == '\n'){
                    break;
                }
            }
            buffer.erase(cursorPosition, pos - cursorPosition);
            editor.UnselectText();
        }
        void AcceptVisitor(CommandVisitor& visitor) override{
            visitor.VisitDeleteWordCommand(*this);
        }
};

/* Макрос */
class MacroCommand : public ICommand {
    std::list<CommandPtr> subcmds_;
    public:
        MacroCommand(std::list<CommandPtr> subcmds) : subcmds_(subcmds) {}
        void Apply(std::string& buffer, size_t& cursorPosition, std::string& clipboard, TextEditor& editor) override{
            for (auto it = subcmds_.begin(); it != subcmds_.end(); ++it ){
                (*it) -> Apply(buffer, cursorPosition, clipboard, editor);
            }
        }
        void AcceptVisitor(CommandVisitor& visitor) override{ 
            for (auto it = subcmds_.begin(); it != subcmds_.end(); ++it ){
                (*it) -> AcceptVisitor(visitor);
            }
        }
        
};

//using std::shared_ptr<LoggedCommandWrapper> LoggedPtr;
/* Эта функция создает объект команды в соответствии с настройками, которые были заданы в текущем объекта билдера */
CommandPtr CommandBuilder::build(){
    if(this -> type_ == Type::MoveCursorLeft){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MoveCursorLeftCommand> cur =  std::make_shared<MoveCursorLeftCommand>(MoveCursorLeftCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MoveCursorLeftCommand>(MoveCursorLeftCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }
    }else if(this -> type_ == Type::MoveCursorRight){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MoveCursorRightCommand> cur =  std::make_shared<MoveCursorRightCommand>(MoveCursorRightCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MoveCursorRightCommand>(MoveCursorRightCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }    
    }else if(this -> type_ == Type::MoveCursorUp){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MoveCursorUpCommand> cur =  std::make_shared<MoveCursorUpCommand>(MoveCursorUpCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MoveCursorUpCommand>(MoveCursorUpCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }
    }else if(this -> type_ == Type::MoveCursorDown){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MoveCursorDownCommand> cur =  std::make_shared<MoveCursorDownCommand>(MoveCursorDownCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MoveCursorDownCommand>(MoveCursorDownCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }    
    }else if(this -> type_ == Type::SelectText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<SelectTextCommand> cur =  std::make_shared<SelectTextCommand>(SelectTextCommand(this->selectionSize_));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<SelectTextCommand>(SelectTextCommand(this->selectionSize_))));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }    
    }else if(this -> type_ == Type::InsertText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<InsertTextCommand> cur =  std::make_shared<InsertTextCommand>(InsertTextCommand(this->text_));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<InsertTextCommand>(InsertTextCommand(this->text_))));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }   
    }else if(this -> type_ == Type::DeleteText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<DeleteTextCommand> cur =  std::make_shared<DeleteTextCommand>(DeleteTextCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<DeleteTextCommand>(DeleteTextCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }
    }else if(this -> type_ == Type::CopyText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<CopyTextCommand> cur =  std::make_shared<CopyTextCommand>(CopyTextCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<CopyTextCommand>(CopyTextCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }
    }else if(this -> type_ == Type::PasteText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<PasteTextCommand> cur =  std::make_shared<PasteTextCommand>(PasteTextCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<PasteTextCommand>(PasteTextCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }    
    }else if(this -> type_ == Type::UppercaseText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<UppercaseTextCommand> cur =  std::make_shared<UppercaseTextCommand>(UppercaseTextCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<UppercaseTextCommand>(UppercaseTextCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        } 
    }else if(this -> type_ == Type::LowercaseText){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<LowercaseTextCommand> cur =  std::make_shared<LowercaseTextCommand>(LowercaseTextCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<LowercaseTextCommand>(LowercaseTextCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }  
    }else if(this -> type_ == Type::MoveToEnd){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MoveToEndCommand> cur =  std::make_shared<MoveToEndCommand>(MoveToEndCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MoveToEndCommand>(MoveToEndCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }    
    }else if(this -> type_ == Type::MoveToStart){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MoveToStartCommand> cur =  std::make_shared<MoveToStartCommand>(MoveToStartCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MoveToStartCommand>(MoveToStartCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }
    }else if(this -> type_ == Type::DeleteWord){
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<DeleteWordCommand> cur =  std::make_shared<DeleteWordCommand>(DeleteWordCommand());
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<DeleteWordCommand>(DeleteWordCommand())));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }  
    }else {
        if(this->logStreamPtr_ == nullptr){
            std::shared_ptr<MacroCommand> cur = std::make_shared<MacroCommand>(MacroCommand(this->subcommands_));
            return std::dynamic_pointer_cast<ICommand>(cur);
        }else{
            std::shared_ptr<LoggedCommandWrapper> cur = std::make_shared<LoggedCommandWrapper>(LoggedCommandWrapper(*logStreamPtr_, std::make_shared<MacroCommand>(MacroCommand(this->subcommands_))));
            return std::dynamic_pointer_cast<ICommand>(cur);
        } 
    }

}

CommandBuilder::CommandBuilder(){
    this -> logStreamPtr_ = nullptr;
    this -> selectionSize_ = 0;
    this -> subcommands_ = {};
    this -> text_ = "";
    this -> type_ = Type::MoveCursorLeft;
}

/*
    * Указать тип команды, которую надо создать
    * Функция возвращает ссылку на *this, чтобы можно было объединять вызовы методов билдера в цепочку (fluent interface)
    */
CommandBuilder& CommandBuilder::WithType(Type type){
    this -> type_ = type;
    return *this;
}

/*
    * Указать размер выделения. Нужно для создания команды SelectText
    * Функция возвращает ссылку на *this, чтобы можно было объединять вызовы методов билдера в цепочку (fluent interface)
    */
CommandBuilder& CommandBuilder::SelectionSize(size_t selectionSize){
    this -> selectionSize_ = selectionSize;
    return *this;
}

/*
    * Указать текст для ввода. Нужно для создания команды InsertText
    * Функция возвращает ссылку на *this, чтобы можно было объединять вызовы методов билдера в цепочку (fluent interface)
    */
CommandBuilder& CommandBuilder::Text(std::string text){
    this -> text_ = text;
    return *this;
}

/*
    * Включить логирование команды. Используется для оборачивания объекта команды в логирующий декоратор LoggedCommandWrapper
    * Функция возвращает ссылку на *this, чтобы можно было объединять вызовы методов билдера в цепочку (fluent interface)
    */
CommandBuilder& CommandBuilder::LogTo(std::ostream& logStream){
    this -> logStreamPtr_ = &logStream;
    return *this;
}

/*
    * Добавить команду в макрос. Используется для создания команды MacroCommand
    * Функция возвращает ссылку на *this, чтобы можно было объединять вызовы методов билдера в цепочку (fluent interface)
    */
CommandBuilder& CommandBuilder::AddSubcommand(CommandPtr subcommand){
    this -> subcommands_.push_back(subcommand);
    return *this;
}

CommandLoggerVisitor::CommandLoggerVisitor(std::ostream& logStream) : logStream_(logStream){}

void CommandLoggerVisitor::VisitMoveCursorLeftCommand(MoveCursorLeftCommand& command){
    logStream_ << 'h';
}
void CommandLoggerVisitor::VisitMoveCursorRightCommand(MoveCursorRightCommand& command){
    logStream_ << 'l';
}
void CommandLoggerVisitor::VisitMoveCursorUpCommand(MoveCursorUpCommand& command){
    logStream_ << 'k';
}
void CommandLoggerVisitor::VisitMoveCursorDownCommand(MoveCursorDownCommand& command){
    logStream_ << 'j';
}
void CommandLoggerVisitor::VisitSelectCommand(SelectTextCommand& command){
    logStream_ << 'v';
}
void CommandLoggerVisitor::VisitInsertTextCommand(InsertTextCommand& command){
    logStream_ << 'i';
}
void CommandLoggerVisitor::VisitDeleteTextCommand(DeleteTextCommand& command){
    logStream_ << 'd';
}
void CommandLoggerVisitor::VisitCopyTextCommand(CopyTextCommand& command){
    logStream_ << 'y';
}
void CommandLoggerVisitor::VisitPasteTextCommand(PasteTextCommand& command){
    logStream_ << 'p';
}
void CommandLoggerVisitor::VisitUppercaseTextCommand(UppercaseTextCommand& command){
    logStream_ << 'U';
}
void CommandLoggerVisitor::VisitLowercaseTextCommand(LowercaseTextCommand& command){
    logStream_ << 'u';
}
void CommandLoggerVisitor::VisitMoveToEndCommand(MoveToEndCommand& command){
    logStream_ << '$';
}
void CommandLoggerVisitor::VisitMoveToStartCommand(MoveToStartCommand& command){
    logStream_ << '0';
}
void CommandLoggerVisitor::VisitDeleteWordCommand(DeleteWordCommand& command){
    logStream_ << "dE";
}
