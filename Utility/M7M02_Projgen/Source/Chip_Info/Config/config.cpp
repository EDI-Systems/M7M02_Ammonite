

/* Begin Function:Chip_Info::Chip_Info *****************************************
Description : Parse the option section of a particular chip.
Input       : xml_node_t* Node - The option section's XML node.
Output      : None.
Return      : None.
******************************************************************************/
/* void */ Option::Option(xml_node_t* Node)
{
    xml_node_t* Temp;
    ptr_t Start;
    ptr_t End;
    std::unique_ptr<std::string> Str;

    try
    {
        /* Name */
        if((XML_Child(Node,"Name",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("D0100: Name section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("D0101: Name section is empty.");
        this->Name=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Type */
        if((XML_Child(Node,"Type",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("D0102: Type section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("D0103: Type section is empty.");
        Str=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        if(*Str=="Range")
            this->Type=OPTION_RANGE;
        else if(*Str=="Select")
            this->Type=OPTION_SELECT;
        else
            throw std::invalid_argument("D0104: Type is malformed.");

        /* Macro */
        if((XML_Child(Node,"Macro",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("D0105: Macro section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("D0106: Macro section is empty.");
        this->Macro=std::make_unique<std::string>(Temp->XML_Val,(int)Temp->XML_Val_Len);

        /* Range */
        if((XML_Child(Node,"Range",&Temp)<0)||(Temp==0))
            throw std::invalid_argument("D0107: Range section is missing.");
        if(Temp->XML_Val_Len==0)
            throw std::invalid_argument("D0108: Range section is empty.");
        Start=0;
        while(Start<Temp->XML_Val_Len)
        {
            End=Start;
            while(End<Temp->XML_Val_Len)
            {
                if(Temp->XML_Val[End]==',')
                    break;
                End++;
            }
            if(End==Start)
                throw std::invalid_argument("D0109: Range section have an empty value.");
            this->Range.push_back(std::make_unique<std::string>(&Temp->XML_Val[Start],(int)(End-Start)));
            Start=End+1;
        }
        if(this->Type==OPTION_RANGE)
        {
            if(this->Range.size()!=2)
                throw std::invalid_argument("D0110: Range typed option cannot have more or less than two ends specified.");

            try
            {
                Start=std::stoull(*(this->Range[0]),0,0);
                End=std::stoull(*(this->Range[1]),0,0);
            }
            catch(std::exception& Exc)
            {
                throw std::runtime_error(std::string("D0111: Starting point or ending point conversion failure.\n")+Exc.what());
            }
            
            if(End<=Start)
                throw std::runtime_error("D0112: No room for value.");
        }
        else
        {
            /* These options must be valid C identifiers or numbers */
            for(std::unique_ptr<std::string>& Str:this->Range)
            {
                if(Kobj::Check_Name(*Str)!=0)
                {
                    /* Is it a number? */
                    try
                    {
                        std::stoull(*Str,0,0);
                    }
                    catch(std::exception& Exc)
                    {
                        throw std::runtime_error(std::string("D0113: Select typed option's possible values must all be valid C identifiers or numbers.\n")+Exc.what());
                    }
                }
            }
        }
    }
    catch(std::exception& Exc)
    {
        if(this->Name!=nullptr)
            throw std::runtime_error(std::string("Option: ")+*(this->Name)+"\n"+Exc.what());
        else
            throw std::runtime_error(std::string("Option: ")+"Unknown"+"\n"+Exc.what());
    }
}
/* End Function:Option::Option ***********************************************/