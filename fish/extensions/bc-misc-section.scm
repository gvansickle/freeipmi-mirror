(define (commit-power-restore-policy section-name power-restore-policy)
  (fi-set-bmc-power-restore-policy power-restore-policy))

(define misc-keys-validator 
  '(("power_restore_policy" valid-power-restore-policy? get-power-restore-policy commit-power-restore-policy)))
