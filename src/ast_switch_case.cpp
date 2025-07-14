#include "ast_switch_case.hpp"
#include "ast_scope.hpp"
#include <ostream>
#include <map>
#include <algorithm>

namespace ast
{
    void SwitchStatement::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        // ----------------  PREREQS and CODE GENERATION FOR SWITCH ----------------

        if (!GetExpression() || !GetStatement())
        {
            return;
        }

        std::string endLabel = context.GenerateUniqueLabel("end_switch");
        context.NewLoopContext(ControlFlowType::SWITCH, "", endLabel, "");

        // evaluate the switch expression into switch_reg
        int switch_reg = context.AssignRegister(TypeSpecifier::INT); // logic only works with int types
        GetExpression()->EmitRISC(stream, context, switch_reg, TypeSpecifier::INT);

        const Scope *scope = dynamic_cast<const Scope *>(GetStatement()); // getting current switch scope
        if (!scope)
        {
            context.FreeRegister(switch_reg);
            context.EndLoopContext();
            return;
        }

        // get all case/default statements and create their labels
        struct CaseInfo
        {
            const CaseStatement *stmt;
            std::string label;
            size_t position;
        };

        std::vector<CaseInfo> cases;
        const DefaultStatement *defaultStatement = nullptr;
        std::string defaultLabel;
        size_t defaultPosition = 0;
        size_t position = 0;

        // find all case/default statements and their positions - so the fall-through works
        for (const auto &child : scope->GetNodes())
        {
            if (const CaseStatement *caseStmt = dynamic_cast<const CaseStatement *>(child.get()))
            {
                cases.push_back({caseStmt, context.GenerateUniqueLabel("case"), position});
            }
            else if (const DefaultStatement *defStmt = dynamic_cast<const DefaultStatement *>(child.get()))
            {
                defaultStatement = defStmt;
                defaultLabel = context.GenerateUniqueLabel("default");
                defaultPosition = position;
            }
            position++;
        }

        // check where to jump to case labels
        for (const auto &caseInfo : cases)
        {
            int case_reg = context.AssignRegister(TypeSpecifier::INT);
            caseInfo.stmt->GetCondition()->EmitRISC(stream, context, case_reg, TypeSpecifier::INT);
            stream << "beq " << context.GetRegString(switch_reg) << ", "
                   << context.GetRegString(case_reg) << ", " << caseInfo.label << std::endl;
            context.FreeRegister(case_reg);
        }

        // jump to default or end if no cases match
        if (defaultStatement)
        {
            stream << "j " << defaultLabel << std::endl;
        }
        else
        {
            stream << "j " << endLabel << std::endl;
        }

        // ----------------  CODE GENERATION FOR CASES ----------------
        // map of positions to case labels
        std::map<size_t, std::string> positionToLabel;
        for (const auto &caseInfo : cases)
        {
            positionToLabel[caseInfo.position] = caseInfo.label;
        }
        if (defaultStatement)
        {
            positionToLabel[defaultPosition] = defaultLabel;
        }

        // emit risc for all statements in the cases
        position = 0;
        for (const auto &node : scope->GetNodes())
        {
            // If this position has a case/default label, emit it
            auto labelIt = positionToLabel.find(position);
            if (labelIt != positionToLabel.end())
            {
                stream << labelIt->second << ":" << std::endl;
            }

            if (const CaseStatement *caseStmt = dynamic_cast<const CaseStatement *>(node.get()))
            {
                if (caseStmt->GetBody())
                {
                    // for every case
                    caseStmt->GetBody()->EmitRISC(stream, context, destReg, type);
                }
            }
            else if (const DefaultStatement *defStmt = dynamic_cast<const DefaultStatement *>(node.get()))
            {
                if (defStmt->GetBody())
                {
                    // for default
                    defStmt->GetBody()->EmitRISC(stream, context, destReg, type);
                }
            }
            else
            {
                // Regular statements - emit as normal
                node->EmitRISC(stream, context, destReg, type);
            }

            position++;
        }

        stream << endLabel << ":" << std::endl;
        context.FreeRegister(switch_reg);
        context.EndLoopContext();
    }

    void SwitchStatement::Print(std::ostream &stream) const
    {
        stream << "switch (";
        expression_->Print(stream);
        stream << ") {" << std::endl;
        statement_->Print(stream);
        stream << "}" << std::endl;
    };

    void CaseStatement::EmitRISC(std::ostream &, Context &, int, TypeSpecifier) const
    {
        // The actual code emission happens in SwitchStatement::EmitRISC
    }

    void CaseStatement::Print(std::ostream &stream) const
    {
        stream << "case ";
        GetCondition()->Print(stream);
        stream << ": ";
        GetBody()->Print(stream);
    }

    void DefaultStatement::EmitRISC(std::ostream &, Context &, int, TypeSpecifier) const
    {
        // The actual code emission happens in SwitchStatement::EmitRISC
    }

    void DefaultStatement::Print(std::ostream &stream) const
    {
        stream << "default: ";
        GetBody()->Print(stream);
    }
}
