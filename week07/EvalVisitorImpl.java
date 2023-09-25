import java.util.HashMap;
import java.util.Map;

/* added the Impl at the end of the class to avoid it being .gitignored sorry */
public class EvalVisitorImpl extends LabeledExprBaseVisitor<Integer> {
	Map<String, Integer> memory = new HashMap<String, Integer>();

	@Override
	public Integer visitAssign(LabeledExprParser.AssignContext ctx) {
		String id = ctx.ID().getText();
		int value = visit(ctx.expr());
		memory.put(id, value);
		return value;
	}
}
