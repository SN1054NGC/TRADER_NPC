// ============================================================
// FILE: TraderRating.c  (module 3_Game -> shared by client and server)
//
// Survival rating of a character -> trader purchase discount.
// Pure math helpers only: no item/inventory access, no side effects.
//
// Progression model (server authoritative):
//   progress = Clamp01(aliveSeconds / (FullHours * 3600)) ^ Curve
//   discount = Round(MaxDiscount * progress)
// Curve > 1 makes the start slow (e.g. 1.35): a quarter of the target time
// yields ~15% of the discount, half the time ~39%, the full time 100%.
// Max discount is clamped to 20% by the caller config; the helper itself
// accepts any value so the server owner can tune it.
//
// Enforce Script notes: no ternary, no void-as-expression, single-line strings.
// ============================================================
class TraderRating
{
	// version tag written after the vanilla PlayerBase block in OnStoreSave
	static const int SAVE_VERSION = 1;

	static float Clamp01( float v )
	{
		if ( v < 0 )
			return 0;
		if ( v > 1 )
			return 1;
		return v;
	}

	// aliveSeconds -> 0..1 progress towards the full discount
	static float ComputeProgress( float aliveSeconds, int fullHours, float curve )
	{
		if ( fullHours <= 0 )
			return 1;
		if ( aliveSeconds <= 0 )
			return 0;

		float target = fullHours * 3600.0;
		float ratio = aliveSeconds / target;
		ratio = Clamp01( ratio );

		float expo = curve;
		if ( expo <= 0 )
			expo = 1.0;

		float progress = Math.Pow( ratio, expo );
		return Clamp01( progress );
	}

	// 0..maxDiscount percent
	static int ComputeDiscount( float progress, int maxDiscount )
	{
		if ( maxDiscount <= 0 )
			return 0;

		int discount = Math.Round( maxDiscount * Clamp01( progress ) );
		if ( discount < 0 )
			discount = 0;
		if ( discount > maxDiscount )
			discount = maxDiscount;
		return discount;
	}

	// price the player actually pays. Minimum 1 unit so a shop entry never
	// becomes free (protects against zero/negative prices and 100% discounts).
	static int ApplyDiscount( int price, int discountPercent )
	{
		if ( price <= 0 )
			return price;
		if ( discountPercent <= 0 )
			return price;
		if ( discountPercent >= 100 )
			return 1;

		float ratio = discountPercent / 100.0;
		int off = Math.Round( price * ratio );
		int finalPrice = price - off;
		if ( finalPrice < 1 )
			finalPrice = 1;
		return finalPrice;
	}

	// "3d 4h" / "4h 12m" / "12m" - built without the % operator on purpose
	static string FormatSurvived( float seconds )
	{
		int total = Math.Round( seconds );
		if ( total < 0 )
			total = 0;

		int days = total / 86400;
		int rest = total - ( days * 86400 );
		int hours = rest / 3600;
		rest = rest - ( hours * 3600 );
		int minutes = rest / 60;

		if ( days > 0 )
			return "" + days + "#tm_rating_days" + " " + hours + "#tm_rating_hours";

		if ( hours > 0 )
			return "" + hours + "#tm_rating_hours" + " " + minutes + "#tm_rating_minutes";

		return "" + minutes + "#tm_rating_minutes";
	}
};
